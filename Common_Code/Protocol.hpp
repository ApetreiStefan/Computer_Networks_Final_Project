#pragma once

#include<map>
#include<string>

// Structura mesajului trimis de Client -> Server
struct ClientMessage {
    int command_id;       // Ce vrea clientul să facă (din enum-ul de mai sus)
    char payload[4096];    // Datele (ex: titlul cărții, user, pass). Dimensiune fixă pt simplitate.
    int user_id = 0;
};

// Structura răspunsului Server -> Client
struct ServerResponse {
    int status_code;      // 200 = OK, 404 = Not Found, 500 = Error
    char message[4096];    // Mesaj text pentru utilizator
    int user_id = 0;
};

enum CommandType {
    CMD_LOGIN = 1,
    CMD_LOGOUT,
    CMD_VIEW,
    CMD_EXIT,
    CMD_KILL,
    CMD_REGISTER,
    CMD_DUMMY,
    CMD_SEARCH,
    CMD_DELETE_HISTORY,
    CMD_DOWNLOAD,
    CMD_DOWNLOAD_START,
    CMD_RECCOMEND
    
};

static std::map<std::string,int> commands={
    {"login", CMD_LOGIN},
    {"logout", CMD_LOGOUT},
    {"view", CMD_VIEW},
    {"exit", CMD_EXIT},
    {"kill", CMD_KILL},
    {"register", CMD_REGISTER},
    {"search", CMD_SEARCH},
    {"delete_history", CMD_DELETE_HISTORY},
    {"download", CMD_DOWNLOAD},
    {"reccomend", CMD_RECCOMEND}

};

enum StatusCodes{
    STATUS_LOGIN_SUCCESSFUL = 1,
    STATUS_LOGIN_FAILED,
    STATUS_LOGOUT,
    STATUS_REGISTER,
    STATUS_EXIT,
    STATUS_VIEW,
    STATUS_SEARCH,
    STATUS_DELETE_HISTORY,
    STATUS_DOWNLOAD,
    STATUS_DOWNLOAD_FAILED,
    STATUS_RECCOMEND
};