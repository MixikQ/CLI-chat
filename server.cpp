#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define IP ip_port.first
#define PORT ip_port.second

int main(int argc, char const *argv[])
{
// ig useless :)
    // // got ip:port 
    // if (argc <= 1) {
    //     std::cerr << "Usage: ./server \"ip:port\"" << std::endl;
    //     return 1;
    // }
    // std::pair<std::string, std::string> ip_port;
    // // regex 0-255.0-255.0-255.0-255:0-65535
    // const std::regex ip_port_pattern("\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b:\\b((?:[0-9][0-9]?[0-9]?[0-9]?[0-9]?|65535))\\b");
    // try {
    //     std::stringstream ss(argv[1]);
    //     if (!std::regex_match( ss.str(), ip_port_pattern )) { throw std::runtime_error("Wrong IP format"); }
    //     std::string temp;
    //     std::getline(ss, temp, ':'); 
    //     IP = temp;
    //     std::getline(ss, temp, ':');
    //     PORT = temp;
    // } catch (...) { 
    //     std::cerr << "Something went wrong, check ip:port typed correctly" << std::endl;
    //     return 1;
    // }

    // creating server and socket ???
    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error during socket binding" << std::endl;
        return 1;
    }

    if (listen(server, 100) == 0) {
        int client;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        if ((client = accept(server, (struct sockaddr*) &client_addr, &client_addr_len)) < 0) {
            std::cerr << "Error during connecting to client" << std::endl;
            close(server);
            return -1;
        }

        char buffer[1024];
        int bytes_read;

        if ((bytes_read = recv(client, buffer, sizeof(buffer), MSG_PEEK)) == 0) {
            std::cerr << "Nothing is sent" << std::endl;
            return 1;
        }
        std::cout << "Recieved: " << bytes_read << " bytes" << std::endl;
        close(client);
    } else {
        close(server);
        std::cerr << "Error during socket listening" << std::endl;
        return 1; 
    }

    close(server);
    
    return 0;
}
