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
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr,"usage: receiver hostname portnumstr\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for host %s on port %s:\n\n", argv[1], argv[2]);

    int sockFd;
    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        std::string str;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            str = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            str = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", str.c_str(), ipstr);

        if ((sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            std::cout << "didn't make socket and errno is " << errno << std::endl;
            return 3;
        } else {
            std::cout << "made socket, fd is " << sockFd << std::endl;
            break;
        }
    }

    int i;
    for (i = 0; i < 10; i++) {
        if ((status = connect(sockFd, p->ai_addr, p->ai_addrlen)) < 0) {
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
        int numBytes = recv(sockFd, bufAsStr, 99, 0);
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
