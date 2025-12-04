#pragma once 
#include"Libraries.hpp"

class Log {
private:
    std::string name;

public:
    Log(std::string name) : name(name) {}
    void log(std::string message, std::string tip_mesaj);
    void log(std::string message);

};