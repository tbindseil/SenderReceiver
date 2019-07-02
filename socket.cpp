#include <socket.h>

socket::socket(const std::string& host, const std::string& port) :
    _host(host),
    _port(port),
    _addr_info(NULL)
{
    // save *p and note that socket creates a socket but doesn't bind so no biggie
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &res)) != 0) {
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

socket::socket() :
    _host(""),
    _port(""),
    _addr_info(NULL)
{

}

socket::~socket()
{
}
