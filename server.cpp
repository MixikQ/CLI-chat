#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <regex>

#define IP ip_port.first
#define PORT ip_port.second

int main(int argc, char const *argv[])
{
    if (argc <= 1) {
        std::cerr << "Usage: ./server \"ip:port\"" << std::endl;
        return 1;
    }
    std::pair<std::string, std::string> ip_port;
    // regex 0-255.0-255.0-255.0-255:0-65535
    const std::regex ip_port_pattern("\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b:\\b((?:[0-9][0-9]?[0-9]?[0-9]?[0-9]?|65535))\\b");
    try {
        std::stringstream ss(argv[1]);
        if (!std::regex_match( ss.str(), ip_port_pattern )) { throw std::runtime_error("Wrong IP format"); }
        std::string temp;
        std::getline(ss, temp, ':'); 
        IP = temp;
        std::getline(ss, temp, ':');
        PORT = temp;
    } catch (...) { 
        std::cerr << "Something went wrong, check ip:port typed correctly" << std::endl;
        return 1;
    }
    return 0;
}
