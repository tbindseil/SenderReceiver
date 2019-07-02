#include <socket.h>

socket::socket(const std::string& host, const std::string& port) :
    _addr_info(NULL)
{
    // save *p and note that socket creates a socket but doesn't bind so no biggie
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    }

    for (_addr_info = res; _addr_info != NULL; _addr_info = _addr_info->ai_next) {
        void *addr;
        std::string str;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (_addr_info->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)_addr_info->ai_addr;
            addr = &(ipv4->sin_addr);
            str = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_addr_info->ai_addr;
            addr = &(ipv6->sin6_addr);
            str = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(_addr_info->ai_family, addr, _ipstr, sizeof _ipstr);
        printf("  %s: %s\n", str.c_str(), _ipstr);

        if ((_fd = socket(_addr_info->ai_family, _addr_info->ai_socktype, _addr_info->ai_protocol)) < 0) {
            std::cout << "didn't make socket and errno is " << errno << std::endl;
        } else {
            std::cout << "made socket, fd is " << _fd << std::endl;
            break;
        }
    }
}

socket::socket(int accepted_socket_fd) :
    _fd(accepted_socket_fd),
    _addr_info(NULL)
{
}

socket::socket(const socket& toCopy) :
    _fd(toCopy._fd),
    _addr_info(toCopy._addr_info)
{
    for (int i = 0; i < INET6_ADDRSTRLEN; i++)
    {
        _ipstr[i] = toCopy._ipstr[i];
    }
}

socket::~socket()
{
    if (_addr_info != NULL)
    {
        freeaddrinfo(_addr_info); // free the linked list
    }
}

int socket::bind()
{
    return bind(_fd, _addr_info->ad_addr, _addr_info->ai_addrlen);
}

int socket::connect()
{
    return connect(_fd, _addr_info->ai_addr, _addr_info->ai_addrlen);
}

int socket::listen(int backlog)
{
    return listen(_fd, backlog);
}

int socket::accept()
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int new_fd;

    addr_size = sizeof their_addr;
    new_fd = accept(_fd, (struct sockaddr *)&their_addr, &addr_size);
    return socket(new_fd);
}

int socket::recv(char* buff, int buff_size, int flags)
{
    return recv(_fd, buff, buff_size, flags);
}

int socket::send(char* buff, int buff_size, int flags)
{
    return send(_fd, buff, buff_size, flags);
}

void socket::close()
{
    close(_fd);
}
