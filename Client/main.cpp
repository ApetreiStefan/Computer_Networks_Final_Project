
#include "./../Common_Code/Libraries.hpp"


#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
    
    std::cout << "Connected to server. Start typing messages. (Type 'exit' to quit)\n";

    std::string message;
    while (true) {

        std::cout << "-> ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        send(sock, message.c_str(), message.length(), 0);
        
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';
            std::cout << "Server reply: " << buffer << std::endl;
        } else if (valread == 0) {
            std::cout << "Server closed connection." << std::endl;
            break;
        } else {
            perror("read failed");
            break;
        }
    }

    close(sock);
    return 0;
}