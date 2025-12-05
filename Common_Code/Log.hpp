#pragma once 
#include"Libraries.hpp"

class Log {
private:
    std::string name;

public:
    Log(const char*);
    void log(std::string message, std::string tip_mesaj);
    void log(std::string message);

};