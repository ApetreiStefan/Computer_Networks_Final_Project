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
    
        bool checkLogin(const std::string& username, const std::string& password) {
            sqlite3_stmt* stmt;
            std::string sql = "SELECT password FROM users WHERE USERNAME = ?;";
    
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
            bool success = false;
            
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* dbPass = sqlite3_column_text(stmt, 0);
                if (dbPass && password == reinterpret_cast<const char*>(dbPass)) {
                    success = true;
                }
            }

            sqlite3_finalize(stmt);
            return success;
        }


        bool registerUser(const std::string& username, const std::string& password) {
            // Notă importantă: În aplicațiile reale, aici ar trebui să folosești o 
            // funcție de HASH (ex: Argon2, bcrypt) pentru a nu stoca parola reală.
            // Aici folosim 'password' ca placeholder pentru hash-ul tău.
            
            sqlite3_stmt* stmt;
            
            // SQL Injection-proof: folosim '?' pentru a marca locul parametrilor
            std::string sql = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES (?, ?);";
            
            // 1. Prepare (Compilăm șablonul SQL)
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::cerr << "Eroare la pregătirea statement-ului (Register): " << sqlite3_errmsg(db) << std::endl;
                return false;
            }
    
            // 2. Bind (Legăm parametrii la placeholder-uri)
            // Bind pentru primul '?' (USERNAME)
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC); 
            
            // Bind pentru al doilea '?' (PASSWORD)
            sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    
            // 3. Step (Executăm inserarea)
            int rc = sqlite3_step(stmt);
            
            // 4. Finalize (Eliberăm resursele)
            sqlite3_finalize(stmt);
    
            if (rc != SQLITE_DONE) {
                // SQLITE_CONSTRAINT este eroarea tipică dacă USERNAME-ul există deja (datorită UNIQUE NOT NULL)
                if (rc == SQLITE_CONSTRAINT) {
                    std::cerr << "Eroare Register: Utilizatorul '" << username << "' există deja." << std::endl;
                } else {
                    std::cerr << "Eroare la execuția statement-ului (Register): " << sqlite3_errmsg(db) << std::endl;
                }
                return false;
            }
            
            std::cout << "Utilizator '" << username << "' înregistrat cu succes." << std::endl;
            return true;
        }
    };

