
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




int Client::getCommand(){
    mesaj.user_id = user_id;

    std::cout << "-> " << std::flush;
    std::getline(std::cin, message);
    words = parseCommand(message);
    
    mesaj.command_id = checkCommand(words);
    strcpy(mesaj.payload,message.c_str());

    return 0;
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




int Client::checkCommand(std::vector<std::string> w){

    for(auto i : commands){
        if(i.first == w[0]){
            return i.second;
        }
    }
    return -1;
}





int Client::runCommand(){
    switch(raspuns.status_code){
        case -1:
        std::cout << "Comanda invalida!" << std::endl;
        break;

        case STATUS_LOGIN_SUCCESSFUL:
        std::cout<< "[Client]: " << raspuns.message << std::endl;
        user_id = raspuns.user_id;
        break;

        case STATUS_LOGIN_FAILED:
        std::cout<< "[Client]: " << raspuns.message << std::endl;
        break;

        case STATUS_LOGOUT:
        user_id = 0;
        std::cout<< "[Client]: " << raspuns.message << std::endl;
        break;

        case STATUS_EXIT:
        std::cout << "[Client]: " << raspuns.message << std::endl;
        close(client_socket);
        exit(0);

        case STATUS_REGISTER:
        std::cout << "[Client]: " << raspuns.message << std::endl;
        break;

        default:
        break;
    }
    return 0;
}






int Client::run(){
    while (true) {

        getCommand();
        send(client_socket, &mesaj, sizeof(mesaj), 0);
        recv(client_socket, &raspuns, sizeof(raspuns), 0);
        runCommand();

    }

    close(client_socket);
    return 0;
}



