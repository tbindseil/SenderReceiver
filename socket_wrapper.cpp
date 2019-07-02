#include <socket_wrapper.h>

// for struct addrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// for close
#include <unistd.h>

#include <string.h>

socket_wrapper::socket_wrapper(const std::string& host, const std::string& port) :
    _addr_info(NULL)
{
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
            fprintf(stderr, "didn't make socket and errno is %d\n", errno);
        } else {
            break;
        }
    }
}

socket_wrapper::socket_wrapper(int accepted_socket_wrapper_fd) :
    _fd(accepted_socket_wrapper_fd),
    _addr_info(NULL)
{
}

socket_wrapper::socket_wrapper(const socket_wrapper& toCopy) :
    _fd(toCopy._fd),
    _addr_info(toCopy._addr_info)
{
    for (int i = 0; i < INET6_ADDRSTRLEN; i++)
    {
        _ipstr[i] = toCopy._ipstr[i];
    }
}

socket_wrapper::~socket_wrapper()
{
    if (_addr_info != NULL)
    {
        freeaddrinfo(_addr_info); // free the linked list
    }
}

int socket_wrapper::bind()
{
    return ::bind(_fd, _addr_info->ai_addr, _addr_info->ai_addrlen);
}

int socket_wrapper::connect()
{
    return ::connect(_fd, _addr_info->ai_addr, _addr_info->ai_addrlen);
}

int socket_wrapper::listen(int backlog)
{
    return ::listen(_fd, backlog);
}

socket_wrapper socket_wrapper::accept()
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int new_fd;

    addr_size = sizeof their_addr;
    new_fd = ::accept(_fd, (struct sockaddr *)&their_addr, &addr_size);
    return socket_wrapper(new_fd);
}

int socket_wrapper::recv(uint8_t* buff, int buff_size, int flags)
{
    return ::recv(_fd, buff, buff_size, flags);
}

int socket_wrapper::send(uint8_t* buff, int buff_size, int flags)
{
    return ::send(_fd, buff, buff_size, flags);
}

void socket_wrapper::close()
{
    ::close(_fd);
}
