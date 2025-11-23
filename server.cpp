#include "includes.h"

#define IP ip_port.first
#define PORT ip_port.second

int main(int argc, char const *argv[]) {
    if (argc <= 1) {
        std::cerr << "Usage: ./server \"port\"" << std::endl;
        return 1;
    }
    unsigned short int port;
    try {
        port = std::stoi(argv[1]); 
        if (port < 0 || port > 65535) { throw std::runtime_error("Wrong port format"); }
    } catch (...) { 
        std::cerr << "Something went wrong, check port typed correctly" << std::endl;
        return 1;
    }

    // creating server and socket ???
    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error during socket binding" << std::endl;
        return 1;
    }
    while (true)
    {
        if (listen(server, 100) == 0) {
            std::cout << "Server is listening on port " << port << std::endl;
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
            std::cout << buffer << std::endl;
            // close(client);
        } else {
            close(server);
            std::cerr << "Error during socket listening" << std::endl;
            return 1; 
        }
    }
    close(server);
    
    return 0;
}
