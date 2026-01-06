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
    // SQL: Inserăm doar dacă username-ul nu există (coloana username ar trebui să fie UNIQUE)
    const char* sql = "INSERT INTO Users (username, password) VALUES (?, ?);";

    // 1. Pregătim statement-ul
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[DB Error] Register Prepare: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 2. Legăm parametrii (Bind)
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    // 3. Executăm inserarea
    int rc = sqlite3_step(stmt);
    bool success = false;

    if (rc == SQLITE_DONE) {
        success = true;
    }


    sqlite3_finalize(stmt);
    return success;
}
    };

