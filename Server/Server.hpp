#pragma once

#include "./../Common_Code/Libraries.hpp"


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

    char buffer[BUFFER_SIZE];

    Server();

public:
    Server(Server&&) = delete;
    Server(const Server&) = delete;

    int run();
    static Server* GetInstance();

    ~Server();
};