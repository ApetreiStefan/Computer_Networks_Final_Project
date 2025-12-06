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
    for(auto i : GetInstance()->client_states){
        delete i.second;
    }
    delete GetInstance();
}




int Server::processCommand(int fd){

    if(client_states[fd]->state == STATE_IDLE){

        read(fd, &temp, sizeof(temp));
        
        switch(temp){
            case 1:
                std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea login!" << std::endl;
                client_states[fd]->state = STATE_AWAITING_CREDENTIALS;
                break;
            case 2:
                std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea logout!" << std::endl;
                break;
            case 3:
                std::cout << "[Server]: Clientul " << fd-3 << " vrea sa vada cele mai noi carti!" << std::endl;
                break;
            case 4:
                std::cout << "[Server]: Clientul " << fd-3 << " a dat exit!" << std::endl;
                FD_CLR(fd, &master_set);
                temp = 0;
                break;
            case 5:
                std::cout << "[Server]: Clientul " << fd-3 << " m-a omorat!" << std::endl;
                exit(0);
                break;
        }
    }

    else if(client_states[fd]->state == STATE_AWAITING_CREDENTIALS){
        static char* p = NULL;
        static int bytesread = read(fd, &buffer, BUFFER_SIZE);
        buffer[bytesread] = '\0'; 
        p = strtok(buffer, " ");
        username = p;
        p = strtok(NULL, " ");
        password = p;

        if(usersDB.checkLogin(username, password) == true){

        }
        else{
            
        }



    }  
    return 0;
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
            client_states[new_client_fd] = new ClientInfo();

            FD_SET(new_client_fd, &master_set);
        }
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(i, &read_set)) {
                processCommand(i);
            }
        }
    }
    close(server_fd);
    return 0;
}