#include "Server.hpp"


Server::Server(){

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET; // protoco ip v4
    server_address.sin_port = htons(PORT); // aici putem portul
    
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    bind(server_fd,(struct sockaddr*)&server_address, sizeof(server_address)); 

    listen(server_fd, 1);
    
    std::cout << "Listening on port: " << PORT << std::endl;
}





Server* Server::GetInstance(){
    static Server* instance;
    if(instance == NULL){
        instance = new Server;
    }

    return instance;
}






std::vector<std::string> parseCommand(const std::string& input){
    std::vector<std::string> words;
    std::stringstream ss(input);
    std::string word;

    while (ss >> word) {
        words.push_back(word);
    }
    return words;
}





int Server::checkCommand(){

    for(auto i : commands){
        if(i.first == words[1]){
            return i.second;
        }
    }
    return -1;
}




int Server::processCommand(int fd){
    message = mesaj.payload;
    words = parseCommand(message);
        
    switch(mesaj.command_id){
        case CMD_LOGIN:
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea login!" << std::endl;
            //cod pentru verificare user
            raspuns.status_code = CMD_DUMMY;
            strcpy(raspuns.message, "dummy");
            break;
        case CMD_LOGOUT:
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea logout!" << std::endl;
            raspuns.status_code = CMD_DUMMY;
            strcpy(raspuns.message, "dummy");
            break;
        case CMD_VIEW:
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa vada cele mai noi carti!" << std::endl;
            raspuns.status_code = CMD_DUMMY;
            strcpy(raspuns.message, "dummy");
            break;
        case CMD_EXIT:
            std::cout << "[Server]: Clientul " << fd-3 << " a dat exit!" << std::endl;
            raspuns.status_code = CMD_EXIT;
            strcpy(raspuns.message, "Ai fost deconectat cu succes!");
            break;
        case 5:
            std::cout << "[Server]: Clientul " << fd-3 << " m-a omorat!" << std::endl;
            raspuns.status_code = CMD_DUMMY;
            strcpy(raspuns.message, "dummy");
            exit(0);
            break;

        default:
        break;
    }
    return 0;
}





int Server::run(){
    static int temp_addrlen = 0;
    static int new_client_fd = 0;
    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    max_fd=server_fd;
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
        for (int i = 4; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(i, &read_set)) {

                int bytes_recieved = recv(i, &mesaj, sizeof(mesaj), 0);

                if(bytes_recieved == 0){
                    close(i);
                    FD_CLR(i, &master_set);
                }

                if(bytes_recieved == sizeof(mesaj)){
                    processCommand(i);
                    send(i, &raspuns, sizeof(raspuns), 0);
                }
            }
        }
    }
    close(server_fd);
    return 0;
}