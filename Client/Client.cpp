
#include "Client.hpp"

Client::Client(){
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




int Client::run(){
    while (true) {

        std::cout << "-> " << std::flush;
        std::getline(std::cin, message);
        std::flush(std::cout); // da, sunt 2 flushuri aici, dar nu repar ce nu e stricat

        if (message == "exit") {
            break;
        }

        send(client_socket, message.c_str(), message.length(), 0);
    }

    close(client_socket);
    return 0;
}



