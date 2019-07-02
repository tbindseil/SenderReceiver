#pragma once

#include <string>

// for INET6_ADDRSTRLEN
#include <arpa/inet.h>

class socket_wrapper {
public:
    socket_wrapper(const std::string& host, const std::string& port);
    socket_wrapper(int fd);
    socket_wrapper(const socket_wrapper& toCopy);
    socket_wrapper();

    ~socket_wrapper();

    int bind();

    int connect();

    int listen(int backlog);

    socket_wrapper accept();

    int recv(uint8_t* buff, int buff_size, int flags);

    int send(uint8_t* buff, int buff_size, int flags);

    void close();

    int get_fd() { return _fd; }

private:
    int _fd;

    struct addrinfo* _addr_info;

    char _ipstr[INET6_ADDRSTRLEN];
};
