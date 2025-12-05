

#include "./../Common_Code/Libraries.hpp"


#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 24


int main() {
    int server_fd = 0, client_fd = 0;
    int max_fd, activity, addrlen;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    std::string message;

    int client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_in client_addresses[MAX_CLIENTS] = {0};

    fd_set master_set;
    fd_set read_set;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0); //socket de TCP

    server_address.sin_family = AF_INET; // protoco ip v4
    server_address.sin_port = htons(PORT); // aici putem portul
    
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr); // aici adaugam adresa ai pi
    
    bind(server_fd,(struct sockaddr*)&server_address, sizeof(server_address)); // aici dam bind la adresa si port la socket
    
    listen(server_fd, 1);
    
    std::cout << "Listening on port: " << PORT << "\n";

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    max_fd=server_fd;
    while (true) {
        read_set = master_set;
        
        activity = select(max_fd + 1, &read_set, NULL, NULL, NULL);

        if ((activity < 0)) {
            std::cout << "Select error: " << "\n";
            continue;
        }
        if (FD_ISSET(server_fd, &read_set)) {
            addrlen = sizeof(server_address);
            client_fd = accept(server_fd, (struct sockaddr *)&server_address, (socklen_t *)&addrlen);

            
            if (client_fd > max_fd)
                max_fd = client_fd;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_fd;
                    client_addresses[i] = server_address;
                    break;
                }
            }
            std::cout << "New connection. SD: " << client_fd << ", IP: " << inet_ntoa(server_address.sin_addr) << "\n";

            FD_SET(client_fd, &master_set);
        }
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
