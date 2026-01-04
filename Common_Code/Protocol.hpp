#pragma once

#include<map>
#include<string>

// Structura mesajului trimis de Client -> Server
struct ClientMessage {
    int command_id;       // Ce vrea clientul să facă (din enum-ul de mai sus)
    char payload[256];    // Datele (ex: titlul cărții, user, pass). Dimensiune fixă pt simplitate.
    int user_id = 0;
};

// Structura răspunsului Server -> Client
struct ServerResponse {
    int status_code;      // 200 = OK, 404 = Not Found, 500 = Error
    char message[256];    // Mesaj text pentru utilizator
    int user_id = 0;
};

enum CommandType {
    CMD_LOGIN = 1,
    CMD_LOGOUT,
    CMD_VIEW,
    CMD_EXIT,
    CMD_KILL,
    CMD_REGISTER,
    CMD_DUMMY
    
};

static std::map<std::string,int> commands={
    {"login", CMD_LOGIN},
    {"logout", CMD_LOGOUT},
    {"view", CMD_VIEW},
    {"exit", CMD_EXIT},
    {"kill", CMD_KILL},
    {"register", CMD_REGISTER}
};

enum StatusCodes{
    STATUS_LOGIN_SUCCESSFUL = 1,
    STATUS_LOGIN_FAILED,
    STATUS_LOGOUT
};