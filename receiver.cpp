#include <receiver.h>
#include <iostream>

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr,"usage: receiver hostname portnumstr\n");
        return 1;
    }

    int status;
    socket socket_obj(argv[1], argv[0]);

    int i;
    for (i = 0; i < 10; i++) {
        if ((status = connect(socket_obj.get_fd(), p->ai_addr, p->ai_addrlen)) < 0) {
            std::cout << "didn't connect socket and errno is " << strerror(errno) << std::endl;
        } else {
            std::cout << "connect socket" << std::endl;
            break;
        }
        sleep(10);
    }
    if (i == 10) {
        std::cout << "didn't connect after 10 tries" << std::endl;
        return 4;
    }

    char bufAsStr[100];
    bufAsStr[0] = '\0';
    while (1) {
        // receive data
        int numBytes = recv(socket_obj.get_fd(), bufAsStr, 99, 0);
        if (numBytes < 0) {
            std::cout << "error with recv errno is " << strerror(errno) << std::endl;
            return 5;
        }
        bufAsStr[numBytes] = '\0';

        std::cout << "got msg:" << bufAsStr << std::endl;

        if (strcmp(bufAsStr, "end") == 0) {
            break;
        }
    }

    freeaddrinfo(res); // free the linked list

    return 0;
}
