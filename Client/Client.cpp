
#include "Client.hpp"

Client::Client(){
    this->initializeMap();

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    std::cout << "Connected to server. Start typing messages. (Type 'exit' to quit)" << std::endl;
}





Client* Client::GetInstance(){
    static Client* instance;
    if(instance == NULL){
        instance = new Client;
    }

    return instance;
}




int Client::getCommand(){
    std::cout << "-> " << std::flush;
    std::getline(std::cin, message);

    return 0;
}





void Client::initializeMap(){
    commands["login"] = 1;
    commands["logout"] = 2;
    commands["view"] = 3;
    commands["exit"] = 4;
    commands["kill"] = 5;
    commands["register"] = 6;
}




int Client::checkCommand(){
    for(auto i : commands){
        if(i.first == message){
            return i.second;
        }
    }
    return -1;
}




int Client::sendCommand(int id){
    if(send(client_socket, &id, sizeof(id), 0) == 0){
        std::cout << "Ceva s-a intamplat cu serverul... OVER!" << std::endl;
    }

    return 0;
}





int Client::run(){
    int temp;
    while (true) {

        getCommand();
        temp = checkCommand();
        if(temp < 0){
            std::cout << "Comanda invalida!" << std::endl;
        }
        else if(temp == 4){
            std::cout << "Ai fost deconectat cu succes!" << std::endl;
            close(client_socket);
            exit(0);
        }
        else sendCommand(temp);
    }

    close(client_socket);
    return 0;
}



