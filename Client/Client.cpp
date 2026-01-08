
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
    words = parseCommand(raspuns.message);
    own_words = parseCommand(mesaj.payload);

    switch(raspuns.status_code){
        case -1: {
        std::cout << "Comanda invalida!" << std::endl;
        break;
        }

        case STATUS_LOGIN_SUCCESSFUL: {
        std::cout<< "[Client]: " << raspuns.message << std::endl;
        user_id = raspuns.user_id;
        break;
        }

        case STATUS_LOGIN_FAILED: {
        std::cout<< "[Client]: " << raspuns.message << std::endl;
        break;
        }

        case STATUS_LOGOUT: {
        user_id = 0;
        std::cout<< "[Client]: " << raspuns.message << std::endl;
        break;
        }

        case STATUS_EXIT: {
        std::cout << "[Client]: " << raspuns.message << std::endl;
        close(client_socket);
        exit(0);
        }

        case STATUS_REGISTER: {
        std::cout << "[Client]: " << raspuns.message << std::endl;
        break;
        }

        case STATUS_VIEW: {
        std::cout << "[Client]: " << raspuns.message << std::endl;
        break;
        }

        case STATUS_SEARCH:{
            std::cout << "[Client]: " << raspuns.message << std::endl;
            break;
        }

        case STATUS_DELETE_HISTORY: {
            std::cout << "[Client]: " << raspuns.message << std::endl;
        }

        case STATUS_DOWNLOAD_FAILED: {
            std::cout << "[Client]: " << raspuns.message << std::endl;
        break;    
        }

        case STATUS_DOWNLOAD: {
            std::cout << "[Client]: " << "Incepe downloadul!" << std::endl;

            if(own_words.size() < 2){
                std::cout << "[Client]: " << "Problema la words!" << std::endl;
                break;
            }

            mesaj.command_id = CMD_DOWNLOAD_START;
            send(client_socket, &mesaj, sizeof(mesaj), 0);

            std::string fileName = "Downloads/descarcat_" + own_words[1];
            FILE *file = fopen(fileName.c_str(), "wb");

            char buffer[1024];
            int bytesReceived;
            while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
                fwrite(buffer, 1, bytesReceived, file);
                if (bytesReceived < sizeof(buffer)){
                    std::cout << "[Client]: " << "Download finalizat!" << std::endl;
                    break;
                }  
            }

        break;
        }

        case STATUS_RECCOMEND: {
            std::cout << "[Client]: " << raspuns.message << std::endl;
        break;
        }

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



