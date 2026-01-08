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






#include <algorithm>
#include <cctype>

std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> words;
    std::stringstream ss(input);
    std::string word;

    while (ss >> word) {
        word.erase(word.begin(), std::find_if(word.begin(), word.end(), [](unsigned char ch) {
            return !std::isspace(ch) && std::isprint(ch);
        }));
        
        word.erase(std::find_if(word.rbegin(), word.rend(), [](unsigned char ch) {
            return !std::isspace(ch) && std::isprint(ch);
        }).base(), word.end());

        if (!word.empty()) {
            words.push_back(word);
        }
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
        
    int id_temp;
    int ok_temp;
    std::string string_temp;
    switch(mesaj.command_id){
        case CMD_LOGIN: {
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea login!" << std::endl;

            if(words.size() < 3) {
                strcpy(raspuns.message, "Format comanda invalid!");
                return -1;
            }

            if(mesaj.user_id !=0){ // clientul este deja logat
                raspuns.user_id = active_sessions[fd];
                raspuns.status_code = STATUS_LOGIN_FAILED;
                strcpy(raspuns.message, "Esti deja logat!");
            break;
            }
            id_temp = DB.checkLogin(words[1], words[2]);
            if(id_temp != 0){
                raspuns.status_code = STATUS_LOGIN_SUCCESSFUL;
                raspuns.user_id = id_temp;
                active_sessions[fd] = id_temp;
                strcpy(raspuns.message, "Ai fost logat cu succes!");
            }
            else{
                raspuns.user_id = 0;
                raspuns.status_code = STATUS_LOGIN_FAILED;
                strcpy(raspuns.message, "Nu ai putut fi logat!");
            }
            break;
        }

        case CMD_LOGOUT: {
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea logout!" << std::endl;
            if(active_sessions.find(fd) == active_sessions.end()){
                strcpy(raspuns.message, "Nu esti logat!");
            }
            else{
                strcpy(raspuns.message, "Ai fost deconectat cu succes!");
            }
            active_sessions.erase(fd);
            raspuns.status_code = STATUS_LOGOUT;
            raspuns.user_id = 0;            
            break;
        }

        case CMD_VIEW: {
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa vada cele mai noi carti!" << std::endl;

            ok_temp = 1;
            if(words.size() > 1){
                for(auto i : words[1]){
                    if(!isdigit(i)){
                        ok_temp = 0;
                    }
                }
            }
            else{
                ok_temp = 0;
            }
            if(ok_temp == 1){
                DB.viewBooks(stoi(words[1]), raspuns.message);
            }
            else{
                DB.viewBooks(10, raspuns.message);
            }
            raspuns.status_code = STATUS_VIEW;
            break;
        }

        case CMD_EXIT: {
            std::cout << "[Server]: Clientul " << fd-3 << " a dat exit!" << std::endl;
            raspuns.status_code = STATUS_EXIT;
            strcpy(raspuns.message, "Ai fost deconectat cu succes!");
            break;
        }
            
        case CMD_REGISTER: {
            std::cout << "[Server]: Clientul " << fd-3 << " vrea sa dea register!" << std::endl;
            if(DB.registerUser(words[1], words[2]) == 1){
                string_temp = "[Server]: Userul " + words[1] + " a fost inregistrat cu succes!";
            }
            else{
                string_temp = "[Server]: A aparut o problema in inregistrarea userului " + words[1];
            }
            raspuns.status_code = STATUS_REGISTER;
            strcpy(raspuns.message,string_temp.c_str());
            break;
        }

        case CMD_KILL: {
            std::cout << "[Server]: Clientul " << fd-3 << " m-a omorat!" << std::endl;
            raspuns.status_code = CMD_DUMMY;
            strcpy(raspuns.message, "dummy");
            exit(0);
            break;
        }

        case CMD_SEARCH: {
            std::cout << "[Server]: Clientul " << fd-3 << " cauta carti..." << std::endl;
            if(active_sessions[fd]==0){
                strcpy(raspuns.message,"Trebuie sa fii logat pentru a folosi aceasta comanda! Logheaza-te sau incearca view!");
                return 1;
            }
            
            std::string s_title = "", s_author = "", s_genre = "", s_isbn = "";
            int s_year = 0;
            float s_rating = 0.0;

            // Parsăm argumentele: search title=Dune_Mantuitorul author=Herbert
            for (size_t i = 1; i < words.size(); ++i) {
                size_t pos = words[i].find('=');
                if (pos != std::string::npos) {
                    std::string key = words[i].substr(0, pos);
                    std::string val = words[i].substr(pos + 1);

                    // Înlocuim '_' cu ' ' pentru a permite căutarea numelor compuse
                    std::replace(val.begin(), val.end(), '_', ' ');

                    if (key == "title") s_title = val;
                    else if (key == "author") s_author = val;
                    else if (key == "genre") s_genre = val;
                    else if (key == "isbn") s_isbn = val;
                    else if (key == "year") {
                        try { s_year = std::stoi(val); } catch(...) { s_year = 0; }
                    }
                    else if (key == "rating") {
                        try { s_rating = std::stof(val); } catch(...) { s_rating = 0.0; }
                    }
                }
            }

            // Executăm căutarea în DB
            DB.searchBooks(s_genre, s_author, s_title, s_year, s_rating, s_isbn, raspuns.message);
            
            // Logăm căutarea doar dacă utilizatorul este logat
            if (active_sessions.find(fd) != active_sessions.end()) {
                // Logăm întreg payload-ul pentru a analiza ulterior preferințele
                DB.logSearch(active_sessions[fd], mesaj.payload);
            }

            raspuns.status_code = STATUS_SEARCH;
            break;
        }
            
        case CMD_DELETE_HISTORY: {
            std::cout << "[Server]: Clientul " << fd-3 << " cu user id " << active_sessions[fd] <<  " doreste sa isi stearga istoricul!" << std::endl;
            raspuns.status_code = STATUS_DELETE_HISTORY;
            if(active_sessions[fd] == 0){
                strcpy(raspuns.message, "Trebuie sa fi logat pentru a putea sterge un istoric!");
                break;
            }
            if(DB.deleteSearch(active_sessions[fd]) == 0){
                strcpy(raspuns.message, "Istoricul a fost sters cu succes!");
            }
            else{
                strcpy(raspuns.message, "Ceva nu a mers bine cu stergerea istoricului!");
            }
            break;
        }

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
            active_sessions[new_client_fd] = 0;

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
                    active_sessions.erase(i);
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