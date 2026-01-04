#pragma once

#include "Libraries.hpp"
#include "./../Common_Code/Protocol.hpp"

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

class Client{
private:
    int user_id = 0;

    int client_socket = 0;
    sockaddr_in serv_addr;
    char write_buffer[BUFFER_SIZE] = {0};
    char read_buffer[BUFFER_SIZE] = {0};

    std::string message;
    std::vector<std::string> words;
    ClientMessage mesaj;
    ServerResponse raspuns;

    int getCommand();
    int checkCommand(std::vector<std::string> w);
    int runCommand();

    Client();
public:
    Client(Client&&) = delete;
    Client(const Client&) = delete;

    static Client* GetInstance();

    int run();
};