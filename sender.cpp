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
#include <socket_wrapper.h>

std::mutex m;
std::condition_variable cv;
std::queue<std::unique_ptr<draw_cmd>> to_send;

int circle_packet_size = 20;
int circle_packet_type = 1;

void send_func(std::shared_ptr<socket_wrapper> new_sock)
{
    std::unique_ptr<draw_cmd> curr_cmd;
    while (1) {
        // check condition
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, []{ return !to_send.empty(); });

        curr_cmd = std::move(to_send.front());
        to_send.pop();

        lock.unlock();

        int bytesSent = new_sock->send(curr_cmd->get_network_buff_start(), draw_cmd::needed_buff_size, 0);
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

void recv_func(std::shared_ptr<socket_wrapper> new_sock)
{
    uint8_t buffer[draw_cmd::needed_buff_size];

    uint32_t type;
    uint32_t radius;
    uint32_t x;
    uint32_t y;
    uint32_t color;

    while (1) {
        unsigned int num_bytes = new_sock->recv(buffer, draw_cmd::needed_buff_size, 0);

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
    if (argc != 3) {
        fprintf(stderr,"usage: sender hostname portnumstr\n");
        return 1;
    }

    int status;
    socket_wrapper listen_sock(argv[1], argv[0]);

    if ((status = listen_sock.bind()) < 0) {
        std::cout << "didn't bind socket and errno is " << errno << std::endl;
        return 4;
    }

    int backlog = 1;
    if ((status = listen_sock.listen(backlog)) != 0) {
        std::cout << "didn't listen socket and errno is " << errno << std::endl;
        return 5;
    }

    std::shared_ptr<socket_wrapper> new_sock = std::make_shared<socket_wrapper>(listen_sock.accept());

    // once a new fd is obtained, how to populate the socket objects stuff
    if (new_sock->get_fd() < 1) {
        std::cout << "didn't accept and errno is " << errno << std::endl;
        return 6;
    }

    // spawn queue driven sender thread
    std::thread sender(&send_func, new_sock);

    // spawn cmd line sender
    std::thread cmd_line_sender(&cmd_line_send_func);

    // spawn receiver
    std::thread receiver(&recv_func, new_sock);

    while (1) {
        // wait to get canceled
        // probably shouldn't spin wait...
    }

    listen_sock.close();
    new_sock->close();

    return 0;
}
