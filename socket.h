#pragma once

#include <string>

class socket {
public:
    socket(const std::string& host, const std::string& port);
    socket(const socket& toCopy);
    socket();

    ~socket();

    int bind(p->ai_addr, p->ai_addrlen);

    int connect();

    int listen(int backlog);

    socket accept();

    int recv(char* buff, int buff_size, int flags);

    int send(char* buff, int buff_size, int flags);

    void close();

    int get_fd() { return _fd; }

private:
    int _fd;

    struct addrinfo* _addr_info;

    char _ipstr[INET6_ADDRSTRLEN];
};
