#include "Server.hpp"



Server::Server(){

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET; // protoco ip v4
    server_address.sin_port = htons(PORT); // aici putem portul
    
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    bind(server_fd,(struct sockaddr*)&server_address, sizeof(server_address)); 

    listen(server_fd, 1);

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    max_fd=server_fd;
    
    std::cout << "Listening on port: " << PORT << std::endl;
}





Server* Server::GetInstance(){
    static Server* instance;
    if(instance == NULL){
        instance = new Server;
    }

    return instance;
}






Server::~Server(){
    delete GetInstance();
}





int Server::run(){
    int temp_addrlen = 0;
    int new_client_fd = 0;
    while (true) {
        read_set = master_set;
        
        select(max_fd + 1, &read_set, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &read_set)) {
            temp_addrlen = sizeof(server_address);
            new_client_fd = accept(server_fd, (struct sockaddr *)&server_address, (socklen_t *)&temp_addrlen);

            if (new_client_fd > max_fd){
                max_fd = new_client_fd;
            }
            std::cout << "New connection" << std::endl;

            FD_SET(new_client_fd, &master_set);
        }
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(i, &read_set)) {
                memset(buffer,0, BUFFER_SIZE);
                
                int nr_read = read(i, buffer, BUFFER_SIZE);
                if (nr_read > 0) {
                    buffer[nr_read] = '\0';
                    std::cout << "[SERVER]: Client said: " << buffer << std::endl;
                } else if (nr_read == 0) {
                    std::cout << "[SERVER]: Client died or smth." << std::endl;
                    FD_CLR(i, &master_set);
                    break;
                }

                if(strcmp(buffer,"kill")==0){
                    std::cout << "[SERVER]: Ma inchid acum, bye bye!" << std::endl;
                    exit(0);
                }
            }
        }
    }
    close(server_fd);
    return 0;
}