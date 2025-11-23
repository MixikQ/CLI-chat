#include "includes.h"

#define IP ip_port.first
#define PORT ip_port.second

int main(int argc, char const *argv[])
{
    // got ip:port 
    if (argc <= 1) {
        std::cerr << "Usage: ./server \"port\"" << std::endl;
        return 1;
    }
    std::pair<std::string, unsigned short int> ip_port;
    // regex 0-255.0-255.0-255.0-255:0-65535
    const std::regex ip_port_pattern("\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b:\\b((?:[0-9][0-9]?[0-9]?[0-9]?[0-9]?|65535))\\b");
    try {
        std::stringstream ss(argv[1]);
        if (!std::regex_match( ss.str(), ip_port_pattern )) { throw std::runtime_error("Wrong IP format"); }
        std::string temp;
        std::getline(ss, ip_port.first, ':'); 
        std::getline(ss, temp, ':');
        ip_port.second = std::stoi(temp);
    } catch (...) { 
        std::cerr << "Something went wrong, check port typed correctly" << std::endl;
        return 1;
    }

    int client = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP.c_str());

    if (connect(client, (sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cerr << "Error during connection" << std::endl;
        close(client);
        return 1;
    }

    char msg[] = "hello there!";
    if (send(client, &msg, sizeof(msg), 0) == -1) {
        std::cerr << "Error during sending" << std::endl;
        close(client);
        return 1;
    }

    close(client);
    return 0;
}
