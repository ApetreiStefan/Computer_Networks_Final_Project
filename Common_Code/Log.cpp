#include"Log.hpp"

void Log::log(std::string message){
    std::cerr << '[' << name << "]: " << message << std::endl; 
}

void Log::log(std::string message, std::string tip_mesaj){
    std::cerr << '[' << name << '|' << tip_mesaj << "]: " << message << std::endl; 
}