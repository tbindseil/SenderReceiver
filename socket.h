#pragma once

#include <string>

class socket {
public:
    socket(const std::string& host, const std::string& port);
    socket(const socket& toCopy);
    socket();

    ~socket();

    int bind(p->ai_addr, p->ai_addrlen);

    int get_fd() { return _fd; }

private:
    int _fd;

    const std::string _host, _post;

    struct addrinfo* _addr_info;

    char _ipstr[INET6_ADDRSTRLEN];
};
