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
        // Folosim TRIM pentru a ignora eventuale spații accidentale
        std::string sql = "SELECT id, password FROM users WHERE TRIM(USERNAME) = ?;";
    
        // 1. Pregătirea statement-ului
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "[DB Error]: Prepare failed: " << sqlite3_errmsg(db) << std::endl;
            return 0;
        }
    
        // 2. Bind parameter (username)
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
        int userId = 0; 
        
        // 3. Execuție
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            // Preluăm ID-ul
            int id = sqlite3_column_int(stmt, 0);
            
            // Preluăm parola din DB și o convertim sigur la std::string
            const unsigned char* dbPassRaw = sqlite3_column_text(stmt, 1);
            std::string dbPass = dbPassRaw ? reinterpret_cast<const char*>(dbPassRaw) : "";
    
            // DEBUG: Scoate aceste linii după ce rezolvi problema
            // std::cout << "DEBUG: DB_Pass=[" << dbPass << "] Input_Pass=[" << password << "]" << std::endl;
    
            if (!dbPass.empty() && password == dbPass) {
                userId = id; // Succes!
            } else {
                std::cout << "[Login]: Parola incorecta pentru user: " << username << std::endl;
            }
        } else if (rc == SQLITE_DONE) {
            std::cout << "[Login]: Utilizatorul nu a fost gasit: " << username << std::endl;
        } else {
            std::cerr << "[DB Error]: Step failed: " << sqlite3_errmsg(db) << std::endl;
        }
    
        // 4. Curățare
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
    // Selectăm titlul și autorul primelor n cărți
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

    // Copiem rezultatul inapoi in buffer-ul message (asigura-te ca e destul de mare!)
    strncpy(message, result.c_str(), 1024); // Presupunem BUFFER_SIZE 1024
    message[1023] = '\0'; // Asiguram terminarea string-ului

    sqlite3_finalize(stmt);
}

    void searchBooks(const std::string& genre, const std::string& author, const std::string& title, 
                                 int year, float minRating, const std::string& isbn, char* responseBuffer) {
    sqlite3_stmt* stmt;
    // Baza query-ului
    std::string sql = "SELECT DISTINCT B.title, A.name, B.release_year, B.rating "
                      "FROM Books B "
                      "JOIN Authors A ON B.author_id = A.id "
                      "LEFT JOIN Book_Genres BG ON B.id = BG.book_id "
                      "LEFT JOIN Genres G ON BG.genre_id = G.id "
                      "WHERE 1=1";

    // Adăugăm clauzele WHERE doar pentru criteriile primite
    if (!title.empty())  sql += " AND B.title LIKE ?";
    if (!author.empty()) sql += " AND A.name LIKE ?";
    if (!genre.empty())  sql += " AND G.name LIKE ?";
    if (year > 0)        sql += " AND B.release_year = ?";
    if (minRating > 0)   sql += " AND B.rating >= ?";
    if (!isbn.empty())   sql += " AND B.isbn = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        strcpy(responseBuffer, "Eroare la procesarea cautarii.");
        return;
    }

    // Facem Bind-ul parametrilor în aceeași ordine
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

    if (!found) output = "Nu s-au gasit carti pentru criteriile introduse.";
    
    strncpy(responseBuffer, output.c_str(), 1024);
    responseBuffer[1023] = '\0';
    sqlite3_finalize(stmt);
    }

    void logSearch(int userId, const std::string& query) {
        sqlite3_stmt* stmt;
        // Query-ul de inserare. Coloana timestamp se completează singură în DB (DEFAULT CURRENT_TIMESTAMP)
        const char* sql = "INSERT INTO SearchHistory (user_id, query) VALUES (?, ?);";

        // 1. Pregătirea statement-ului
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "[DB Error] logSearch Prepare: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // 2. Legăm parametrii (Bind)
        // Primul '?' este user_id (integer)
        sqlite3_bind_int(stmt, 1, userId);
        // Al doilea '?' este query-ul (text)
        sqlite3_bind_text(stmt, 2, query.c_str(), -1, SQLITE_STATIC);

        // 3. Executăm inserarea
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "[DB Error] logSearch Step: " << sqlite3_errmsg(db) << std::endl;
        }

        // 4. Eliberăm resursele
        sqlite3_finalize(stmt);
    }

};

