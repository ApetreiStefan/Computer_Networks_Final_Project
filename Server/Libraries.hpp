#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <sqlite3.h>


std::vector<std::string> parseCommand(const std::string& input);

