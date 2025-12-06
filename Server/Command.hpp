#pragma once

#include "Libraries.hpp"

class Command{
public:
    virtual int execute() = 0;
    virtual ~Command();

};

class Command_Login : public Command{
public:
    virtual int execute();


};