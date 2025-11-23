#include "includes.h"

int main(int argc, char const *argv[])
{
    int client = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(25565);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
