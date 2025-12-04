

#include "./../Common_Code/Libraries.hpp"


#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int server_fd = 0, client_fd = 0;
    struct sockaddr_in server_adress;
    char buffer[BUFFER_SIZE] = {0};
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0); //socket de TCP

    server_adress.sin_family = AF_INET; // protocop ip v4
    server_adress.sin_port = htons(PORT); // aici putem portul
    
    inet_pton(AF_INET, SERVER_IP, &server_adress.sin_addr); // aici adaugam adresa ai pi
    
    bind(server_fd,(struct sockaddr*)&server_adress, sizeof(server_adress)); // aici dam bind la adresa si port la socket
    
    listen(server_fd, 1);

    

    std::string message;
    while (true) {
  
        std::cout << "-> ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        send(server_fd, message.c_str(), message.length(), 0);
        
        int valread = read(server_fd, buffer, BUFFER_SIZE);
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
 
    close(server_fd);
    return 0;
}