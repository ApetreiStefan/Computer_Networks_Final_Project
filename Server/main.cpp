#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // 1. Creare socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convertire adresa IP din text in binar
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }
    
    // 2. Conectare la server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
    
    std::cout << "Connected to server. Start typing messages. (Type 'exit' to quit)\n";

    std::string message;
    while (true) {
        // Citeste mesajul de la utilizator
        std::cout << "-> ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        // 3. Trimite mesajul la server
        send(sock, message.c_str(), message.length(), 0);
        
        // 4. Asteapta si primeste raspunsul de la server
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

    // Inchide socket-ul
    close(sock);
    return 0;
}