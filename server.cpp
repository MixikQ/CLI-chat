#include "includes.h"
#include "IPfuncs.h"

#define BACKLOG 10
#define MAX_MSG_LEN 1024

void quit(int sockfd) {
    while (true) {    
        std::string msg;
        std::getline(std::cin, msg);
        if (msg == "~quit" || msg == "~exit") {
            close(sockfd);
            exit(0);
        }
    }
}

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
    
    int listener;
    struct addrinfo hints, *ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    getaddrinfo(getIP().c_str(), argv[1], &hints, &ai);

    listener = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

    if (listener == -1) {
        perror("socket");
        return 1;   
    }

    int yes = 1;
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }

    if (bind(listener, ai->ai_addr, ai->ai_addrlen) == -1) {
        perror("bind");
        return 1;
    }

    freeaddrinfo(ai);

    std::thread wait_for_input(quit, listener);

    fd_set master, read_fds;
    int fdmax;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    if (listen(listener, BACKLOG) == -1) {
        perror("listen");
        return 1;
    }

    FD_SET(listener, &master);
    fdmax = listener;

    struct sockaddr_storage their_addr;
    int new_fd, recv_bytes;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    std::unordered_map<int, std::string> usernames;
    std::vector<std::string> history;

    while (true) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            return 1;
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    addrlen = sizeof(remoteaddr);
                    new_fd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);
                    if (new_fd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(new_fd, &master);
                        fdmax = (fdmax >= new_fd) ? fdmax : new_fd;
                        for (int u = 0; u < 5; u++) {
                            if (send(new_fd, "Connected to server\nType your nickname:", sizeof("Connected to server\nType your nickname:"), 0) == -1) {
                                perror("send");
                            }
                            sleep(1);
                        }
                    }
                } else {
                    char buffer[MAX_MSG_LEN] = {};
                    recv_bytes = recv(i, buffer, sizeof(buffer), 0);
                    if (recv_bytes <= 0) {
                        if (recv_bytes == 0) {
                            std::cerr << "Conncetion closed by client" << std::endl;
                        } else {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master);
                        usernames.erase(i);
                    } else {
                        if (usernames.find(i) == usernames.end()) {
                            usernames[i] = buffer;
                        } else {
                            strcpy(buffer, (usernames[i] + ": " + std::string(buffer)).c_str());
                            std::cout << buffer << std::endl;
                            for (int j = 0; j <= fdmax; j++) {
                                if (FD_ISSET(j, &master)) {
                                    if (j != listener && j != i) {
                                        if (send(j, buffer, sizeof(buffer), 0) == -1) {
                                            perror("send");
                                        }
                                    }
                                }
                            }
                        }
                    }
                } 
            }
        }
    }
    wait_for_input.join();
    return 0;
}
