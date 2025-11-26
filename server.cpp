#include "includes.h"
#include "IPfuncs.h"

#define BACKLOG 10

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
    
    int sockfd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    getaddrinfo(getIP().c_str(), argv[1], &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd == -1) {
        perror("socket");
        return 1;   
    }

    int yes = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        return 1;
    }

    freeaddrinfo(res);

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        return 1;
    }

    struct sockaddr_storage their_addr;
    int new_fd;

    while (true) {
        socklen_t addr_size = sizeof(their_addr);

        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        } 

        char msg[] = "Connected to server";
        int len = sizeof(msg), bytes_sent;
        if (bytes_sent = send(new_fd, msg, 20, 0) == -1) {
            perror("send");
            return 1;
        }
        while (true) {
            char buffer[1024] = {};
            int buffer_len = sizeof(buffer), bytes_recieved;
            if (bytes_recieved = recv(new_fd, buffer, buffer_len, 0) <= 0) {
                perror("recv");
                break;
            }
            std::cout << buffer << std::endl;

            if (!fork()) {
                close(sockfd);
                close(new_fd);
                return 0;
            }
        }
    }

    return 0;
}
