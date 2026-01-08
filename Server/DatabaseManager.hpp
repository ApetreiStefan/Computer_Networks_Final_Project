#pragma once
#include "Libraries.hpp"

class DatabaseManager {
private:
    sqlite3* db;

public:
    DatabaseManager(const char* dbPath) {

        sqlite3_open(dbPath, &db);
        sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, 0, 0);
    }

    ~DatabaseManager() {
        sqlite3_close(db);
    }

    int checkLogin(const std::string& username, const std::string& password) {
        sqlite3_stmt* stmt;
        std::string sql = "SELECT id, password FROM users WHERE TRIM(USERNAME) = ?;";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "[DB Error]: Prepare failed: " << sqlite3_errmsg(db) << std::endl;
            return 0;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
        int userId = 0; 

        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);

            const unsigned char* dbPassRaw = sqlite3_column_text(stmt, 1);
            std::string dbPass = dbPassRaw ? reinterpret_cast<const char*>(dbPassRaw) : "";

            if (!dbPass.empty() && password == dbPass) {
                userId = id;
            } else {
                std::cout << "[Login]: Parola incorecta pentru user: " << username << std::endl;
            }
        } else if (rc == SQLITE_DONE) {
            std::cout << "[Login]: Utilizatorul nu a fost gasit: " << username << std::endl;
        } else {
            std::cerr << "[DB Error]: Step failed: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
        return userId; 
    }

    bool registerUser(const std::string& username, const std::string& password) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Users (username, password) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[DB Error] Register Prepare: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    bool success = false;
    if (rc == SQLITE_DONE) {
        success = true;
    }
    sqlite3_finalize(stmt);
    return success;
}
    
    void viewBooks(int n, char* message) {
    sqlite3_stmt* stmt;

    const char* sql = "SELECT title, (SELECT name FROM Authors WHERE id = author_id) FROM Books LIMIT ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        strcpy(message, "Eroare la accesarea bazei de date.");
        return;
    }

    sqlite3_bind_int(stmt, 1, n);

    std::string result = "Cele mai noi carti:\n";
    bool found = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        const unsigned char* title = sqlite3_column_text(stmt, 0);
        const unsigned char* author = sqlite3_column_text(stmt, 1);

        result += "- ";
        result += reinterpret_cast<const char*>(title);
        result += " de ";
        result += (author ? reinterpret_cast<const char*>(author) : "Autor necunoscut");
        result += "\n";
    }

    if (!found) {
        result = "Nu exista carti in librarie.";
    }


    strncpy(message, result.c_str(), 1024);
    message[1023] = '\0';

    sqlite3_finalize(stmt);
}

    int searchBooks(const std::string& genre, const std::string& author, const std::string& title, 
                                 int year, float minRating, const std::string& isbn, char* responseBuffer) {
    sqlite3_stmt* stmt;
    std::string sql = "SELECT DISTINCT B.title, A.name, B.release_year, B.rating "
                      "FROM Books B "
                      "JOIN Authors A ON B.author_id = A.id "
                      "LEFT JOIN Book_Genres BG ON B.id = BG.book_id "
                      "LEFT JOIN Genres G ON BG.genre_id = G.id "
                      "WHERE 1=1";

    if (!title.empty())  sql += " AND B.title LIKE ?";
    if (!author.empty()) sql += " AND A.name LIKE ?";
    if (!genre.empty())  sql += " AND G.name LIKE ?";
    if (year > 0)        sql += " AND B.release_year = ?";
    if (minRating > 0)   sql += " AND B.rating >= ?";
    if (!isbn.empty())   sql += " AND B.isbn = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        strcpy(responseBuffer, "Eroare la procesarea cautarii.");
        return -1;
    }

    int bindIdx = 1;
    if (!title.empty())  sqlite3_bind_text(stmt, bindIdx++, ("%" + title + "%").c_str(), -1, SQLITE_TRANSIENT);
    if (!author.empty()) sqlite3_bind_text(stmt, bindIdx++, ("%" + author + "%").c_str(), -1, SQLITE_TRANSIENT);
    if (!genre.empty())  sqlite3_bind_text(stmt, bindIdx++, ("%" + genre + "%").c_str(), -1, SQLITE_TRANSIENT);
    if (year > 0)        sqlite3_bind_int(stmt, bindIdx++, year);
    if (minRating > 0)   sqlite3_bind_double(stmt, bindIdx++, (double)minRating);
    if (!isbn.empty())   sqlite3_bind_text(stmt, bindIdx++, isbn.c_str(), -1, SQLITE_TRANSIENT);

    std::string output = "Rezultate gasite:\n-----------------\n";
    bool found = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        std::string bTitle = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string bAuth  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int bYear          = sqlite3_column_int(stmt, 2);
        double bRating     = sqlite3_column_double(stmt, 3);

        output += "- " + bTitle + " (" + bAuth + ") [" + std::to_string(bYear) + "] Rating: " 
                  + std::to_string(bRating).substr(0, 3) + "\n";
    }

    if (!found){
        output = "Nu s-au gasit carti pentru criteriile introduse.";
        strncpy(responseBuffer, output.c_str(), 1024);
        responseBuffer[1023] = '\0';
        sqlite3_finalize(stmt);
        return -1;
    } 
    
    strncpy(responseBuffer, output.c_str(), 1024);
    responseBuffer[1023] = '\0';
    sqlite3_finalize(stmt);
    return 0;
    }

    void logSearch(int userId, const std::string& query) {
        sqlite3_stmt* stmt;

        const char* sql = "INSERT INTO SearchHistory (user_id, query) VALUES (?, ?);";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "[DB Error] logSearch Prepare: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, query.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "[DB Error] logSearch Step: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
    }

    int deleteSearch(int userID){
        sqlite3_stmt* stmt;

        const char* sql = "DELETE FROM SearchHistory WHERE user_id = ?";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "[DB Error] deleteSearch Prepare: " << sqlite3_errmsg(db) << std::endl;
            return -1;
        }
        
        sqlite3_bind_int(stmt, 1, userID);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "[DB Error] deleteSearch Step: " << sqlite3_errmsg(db) << std::endl;
            return -1;
        }
        

        sqlite3_finalize(stmt);
        return 0;
    }

    bool logDownload(int userId, const std::string& bookTitle, char* pathResponse) {
        sqlite3_stmt* stmt;
        
        const char* sqlFind = "SELECT id, download_path FROM Books WHERE title LIKE ? LIMIT 1;";
        
        if (sqlite3_prepare_v2(db, sqlFind, -1, &stmt, nullptr) != SQLITE_OK) return false;
        
        std::string searchPattern = "%" + bookTitle + "%";
        sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_TRANSIENT);
    
        int bookId = -1;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            bookId = sqlite3_column_int(stmt, 0);
            const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            if (path) strcpy(pathResponse, path);
            else strcpy(pathResponse, "Path_not_defined");
        }
        sqlite3_finalize(stmt);
    
        if (bookId == -1) return false;

        const char* sqlLog = "INSERT INTO Downloads (user_id, book_id) VALUES (?, ?);";
        sqlite3_prepare_v2(db, sqlLog, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, bookId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    
        return true;
    }

    bool bookExists(const std::string& bookTitle) {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT id FROM Books WHERE title = ? AND download_path IS NOT NULL;";
    
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "[DB Error] bookExists Prepare: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, bookTitle.c_str(), -1, SQLITE_STATIC);
    
        bool exists = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = true;
        }
    
        sqlite3_finalize(stmt);
        return exists;
    }

    std::string getRecommendations(int userId) {
        sqlite3_stmt* stmt;
        
        std::string sql = 
            "WITH UserGenres AS ("
            "    SELECT DISTINCT genre_id FROM Book_Genres WHERE book_id IN (SELECT book_id FROM Downloads WHERE user_id = ?)"
            "    UNION "
            "    SELECT id FROM Genres WHERE name IN (SELECT query FROM SearchHistory WHERE user_id = ?)"
            "),"
            "Similarity AS ("
            "    SELECT d2.user_id, COUNT(*) as common_books "
            "    FROM Downloads d1 JOIN Downloads d2 ON d1.book_id = d2.book_id "
            "    WHERE d1.user_id = ? AND d2.user_id != ? "
            "    GROUP BY d2.user_id"
            ") "
            "SELECT B.title, "
            "       (B.rating * 2 + "
            "       (SELECT COUNT(*) FROM Book_Genres BG WHERE BG.book_id = B.id AND BG.genre_id IN UserGenres) * 5 + "
            "       (SELECT COUNT(*) FROM Similarity S JOIN Downloads D ON S.user_id = D.user_id WHERE D.book_id = B.id) * 3"
            "       ) as score "
            "FROM Books B "
            "WHERE B.id NOT IN (SELECT book_id FROM Downloads WHERE user_id = ?) "
            "ORDER BY score DESC LIMIT 5;";
    
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return "Eroare la generarea recomandarilor: " + std::string(sqlite3_errmsg(db));
        }

        for(int i = 1; i <= 5; i++) sqlite3_bind_int(stmt, i, userId);
    
        std::string result = "--- Recomandari pentru tine ---\n";
        bool found = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            result += "- " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\n";
        }
    
        sqlite3_finalize(stmt);
        return found ? result : "Inca nu avem destule date pentru a-ti oferi recomandari. Mai exploreaza biblioteca!";
    }
};

