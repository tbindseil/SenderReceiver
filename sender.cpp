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

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

#include <draw_cmd.h>

std::mutex m;
std::condition_variable cv;
std::queue<std::unique_ptr<draw_cmd>> to_send;

int circle_packet_size = 20;
int circle_packet_type = 1;

void send_func(int new_fd)
{
    std::unique_ptr<draw_cmd> curr_cmd;
    while (1) {
        // check condition
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, []{ return !to_send.empty(); });

        curr_cmd = std::move(to_send.front());
        to_send.pop();

        lock.unlock();

        int bytesSent = send(new_fd, curr_cmd->get_network_buff_start(), draw_cmd::needed_buff_size, 0);
        (void)bytesSent;
    }
}

/**
 * @brief enqueue a drawing packet to be send out on the socked
 */
void enqueueSend(std::unique_ptr<draw_cmd> draw_cmd)
{
    draw_cmd->hton();

    // enqueue, then another thread dequeues
    std::unique_lock<std::mutex> lock(m);
    to_send.push(std::move(draw_cmd));
    lock.unlock();
    cv.notify_one();
}

void cmd_line_send_func()
{
    std::cout << "enter draw cmd data: [cmd] [radius] [x] [y] [color]" << std::endl;

    uint32_t infoArray[5]; // type + radius, x, y, color;

    std::string line;
    std::string token;
    while (1) {
        // TODO use getopt
        getline(std::cin, line);

        std::istringstream iss(line);

        int i = 1;
        while (getline(iss, token, ' ')) {
            if (i > 5) { std::cout << "uh oh!" << std::endl; /*return 7;*/ }
            if (token.find("0x") == 0) {
                infoArray[i] = strtol(token.c_str(), NULL, 16);
            } else {
                infoArray[i] = strtol(token.c_str(), NULL, 10);
            }
            i++;
        }

        auto cmd = std::make_unique<draw_cmd>(infoArray[0], infoArray[1], infoArray[2], infoArray[3], infoArray[4]);
        enqueueSend(std::move(cmd));
    }
}

void recv_func(int new_fd)
{
    uint8_t buffer[draw_cmd::needed_buff_size];

    uint32_t type;
    uint32_t radius;
    uint32_t x;
    uint32_t y;
    uint32_t color;

    while (1) {
        unsigned int num_bytes = recv(new_fd, buffer, draw_cmd::needed_buff_size, 0);

        if (num_bytes < draw_cmd::needed_buff_size) {
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

        auto cmd = std::make_unique<draw_cmd>(type, radius, x, y, color);
        enqueueSend(std::move(cmd));
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

    // spawn queue driven sender thread
    std::thread sender(&send_func, new_fd);

    // spawn cmd line sender
    std::thread cmd_line_sender(&cmd_line_send_func);

    // spawn receiver
    std::thread receiver(&recv_func, new_fd);

    while (1) {
        // wait to get canceled
        // probably shouldn't spin wait...
    }

    close(sockFd);
    close(new_fd);
    freeaddrinfo(res); // free the linked list

    return 0;
}
