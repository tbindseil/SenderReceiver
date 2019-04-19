#include <sender.h>
#include <iostream>
#include <sstream>

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <thread>

#include <unistd.h>

int circle_packet_size = 20;
int circle_packet_type = 1;

/**
 * @brief enqueue a drawing packet to be send out on the socked
 */
void enqueueSend(uint32_t type, uint32_t radius, uint32_t x, uint32_t y, uint32_t color)
{

}

void send_func(int new_fd)
{
    std::cout  << "enter circle data: [radius] [x] [y] [color]" << std::endl;

    uint32_t type = circle_packet_type;
    uint32_t typeN = htonl(type);
    uint32_t infoArray[5]; // type + radius, x, y, color;
    uint32_t networkArray[5]; // type + radiusN, xN, yN, colorN;
    networkArray[0] = typeN;

    uint8_t buffer[circle_packet_size]; // four bytes for each of the four things + 4 for a type
    std::string line;
    std::string token;
    while (1) {
        getline(std::cin, line);

        std::istringstream iss(line);

        int i = 1;
        while (getline(iss, token, ' ')) {
            if (i > 4) { std::cout << "uh oh!" << std::endl; /*return 7;*/ }
            if (token.find("0x") == 0) {
                infoArray[i] = strtol(token.c_str(), NULL, 16);
            } else {
                infoArray[i] = strtol(token.c_str(), NULL, 10);
            }
            i++;
        }

        for (int i = 0; i < 4; i++) {
            networkArray[i + 1] = htonl(infoArray[i + 1]);
        }

        for (int i = 0; i < 5; i++) {
            int startIndex = i * 4;
            buffer[startIndex++] = networkArray[i] & 0xff;
            buffer[startIndex++] = (networkArray[i] & 0xff00) >> 8;
            buffer[startIndex++] = (networkArray[i] & 0xff0000) >> 16;
            buffer[startIndex++] = (networkArray[i] & 0xff000000) >> 24;
        }

        int bytesSent = send(new_fd, buffer, circle_packet_size, 0);
        (void)bytesSent;
    }
}

void recv_func(int new_fd)
{
    uint32_t buffLen = 256;
    uint8_t buffer[buffLen];

    uint32_t type; // continue by modulatiing the send functionality,
    uint32_t radius;
    uint32_t x;
    uint32_t y;
    uint32_t color;

    // continue by modulatiing the send functionality,
    while (1) {
        int num_bytes = recv(new_fd, buffer, buffLen, 0);

        if (num_bytes < circle_packet_size) {
            std::cout  << "invalid packet size" << std::endl;
            continue;
        }

        uint32_t uintsRead = 0;
        type = ntohl((uint32_t)buffer[uintsRead * sizeof(uint32_t)]);
        radius = ntohl((uint32_t)buffer[++uintsRead * sizeof(uint32_t)]);
        x = ntohl((uint32_t)buffer[++uintsRead * sizeof(uint32_t)]);
        y = ntohl((uint32_t)buffer[++uintsRead * sizeof(uint32_t)]);
        color = ntohl((uint32_t)buffer[++uintsRead * sizeof(uint32_t)]);

        // manipulate(type, radius, x, y, color);

        enqueueSend(type, radius, x, y, color);
    }
}

int main(int argc, char* argv[])
{
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 3) {
        fprintf(stderr,"usage: sender hostname portnumstr\n");
        return 1;
    }

    // clear and prepare hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for host %s on port %s:\n\n", argv[1], argv[2]);

    int sockFd;
    for (p = res; p != NULL; p = p->ai_next) {
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
            if ((status = bind(sockFd, p->ai_addr, p->ai_addrlen)) < 0) {
                std::cout << "didn't bind socket and errno is " << errno << std::endl;
                return 4;
            } else {
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

    // spawn sender
    std::thread sender(&send_func, new_fd);

    // spawn receiver
    std::thread receiver(&recv_func, new_fd);

    while (1) {
        // wait to get canceled
    }

    close(sockFd);
    close(new_fd);
    freeaddrinfo(res); // free the linked list

    return 0;
}
