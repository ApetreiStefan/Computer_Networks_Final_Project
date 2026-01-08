#pragma once

#include "Libraries.hpp"
#include "DatabaseManager.hpp"
#include "./../Common_Code/Protocol.hpp"


#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 24

class Server{
private:

    int server_fd = 0;
    sockaddr_in server_address;

    fd_set master_set;
    fd_set read_set;
    int max_fd = 0;


    int checkCommand();
    ClientMessage mesaj;
    ServerResponse raspuns;
    std::string message;
    std::vector<std::string> words;

    DatabaseManager DB = DatabaseManager("./Databases/baze_de_date.db");
    std::string username;
    std::string password;
    std::map<int, int> active_sessions;
    int callback_users(void* data, int argc, char** argv, char** colName);
    char filePath[256];

    int command_id;
    int processCommand(int fd);

    Server();

public:
    Server(Server&&) = delete;
    Server(const Server&) = delete;

    int run();
    static Server* GetInstance();
};