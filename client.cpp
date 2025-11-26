#include "includes.h"

#define IP ip_port.first
#define PORT ip_port.second
#define MAX_MSG_LEN 1024

void recv_msg(int sockfd) {
    while (true) {
        char buffer[MAX_MSG_LEN] = {};
        int buffer_len = sizeof(buffer), bytes_recieved;
        if(bytes_recieved = recv(sockfd, buffer, buffer_len, 0) == -1) {
            perror("recv");
            continue;
        }
        std::cout << buffer << std::endl;
    }   
}

void send_msg(int sockfd) {
    while (true) {    
        std::string msg;
        std::getline(std::cin, msg);
        if (msg == "~quit" || msg == "~exit") {
            close(sockfd);
            exit(0);
        }
        int msg_len = strlen(msg.c_str()), bytes_sent;
        if(bytes_sent = send(sockfd, msg.c_str(), msg_len, 0) == -1) {
            perror("send");
            continue;
        }
    }
}

int main(int argc, char const *argv[])
{
    // got ip:port 
    if (argc <= 1) {
        std::cerr << "Usage: ./client \"ip:port\"" << std::endl;
        return 1;
    }
    std::pair<std::string, std::string> ip_port;
    // regex 0-255.0-255.0-255.0-255:0-65535
    const std::regex ip_port_pattern("\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b.\\b((?:[0-9][0-9]?[0-9]?|255))\\b:\\b((?:[0-9][0-9]?[0-9]?[0-9]?[0-9]?|65535))\\b");
    try {
        std::stringstream ss(argv[1]);
        if (!std::regex_match( ss.str(), ip_port_pattern )) { throw std::runtime_error("Wrong IP format"); }
        std::string temp;
        std::getline(ss, ip_port.first, ':'); 
        std::getline(ss, temp, ':');
        ip_port.second = temp;
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

    getaddrinfo(IP.c_str(), PORT.c_str(), &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd == -1) {
        perror("socket");
        return 1;   
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        return 1;
    }

    std::thread recieving(recv_msg, sockfd);
    std::thread sending(send_msg, sockfd);

    recieving.join();
    sending.join();

    return 0;
}
