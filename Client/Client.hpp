#include "./../Common_Code/Libraries.hpp"

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

class Client{
private:
    int client_socket = 0;
    sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    std::string message;
    
    Client();
public:
    Client(Client&&) = delete;
    Client(const Client&) = delete;

    static Client* GetInstance();

    int run();
};