#include <sender.h>
#include <iostream>

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

/*int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints,
                struct addrinfo **res);*/

/*           struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };*/

int main(int argc, char* argv[])
{
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr,"usage: sender hostname portnumstr\n");
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
            if ((status = bind(sockFd, p->ai_addr, p->ai_addrlen)) < 0) {
                std::cout << "didn't bind socket and errno is " << errno << std::endl;
                return 4;
            } else {
                std::cout << "bound socket" << std::endl;
                break;
            }
        }
    }

    int backlog = 1;
    if ((status = listen(sockFd, backlog)) != 0) {
        std::cout << "didn't listen socket and errno is " << errno << std::endl;
        return 5;
    }



    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int new_fd;

    // now accept an incoming connection:

    addr_size = sizeof their_addr;
    new_fd = accept(sockFd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd < 1) {
        std::cout << "didn't accept and errno is " << errno << std::endl;
        return 6;
    }

    // ready to communicate on socket descriptor new_fd!

    std::string read;
    while (1) {
        std::cin >> read;
        if (read.size() > 99) {
            std::cout << "only 99 characters allowed" << std::endl;
            continue;
        }

        send(new_fd, read.c_str(), read.size() + 1, 0);
        if (read.compare("end") == 0) {
            std::cout << "last transmission" << std::endl;
            break;
        }
    }

    freeaddrinfo(res); // free the linked list

    return 0;
}

/*

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // servinfo now points to a linked list of 1 or more struct addrinfos

    int i = 0;
    while (servinfo) {
        i++;
        servinfo = servinfo->ai_next;
        std::cout << "ai_canonname is " << servinfo->ai_canonname << std::endl;
    }
    std::cout << "i is " << i << std::endl;


    // ... do everything until you don't need servinfo anymore ....

    freeaddrinfo(servinfo); // free the linked-list

    return 0;
}*/
