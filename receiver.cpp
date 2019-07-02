#include <receiver.h>
#include <iostream>

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr,"usage: receiver hostname portnumstr\n");
        return 1;
    }

    int status;
    socket socket_obj(argv[1], argv[0]);

    int i;
    for (i = 0; i < 10; i++) {
        if ((status = socket_obj.connect()) < 0) {
            std::cout << "didn't connect socket and errno is " << strerror(errno) << std::endl;
        } else {
            std::cout << "connect socket" << std::endl;
            break;
        }
        sleep(10);
    }
    if (i == 10) {
        std::cout << "didn't connect after 10 tries" << std::endl;
        return 4;
    }


    const uint8_t buff_size = 100;
    char buff[buff_size];
    buff[0] = '\0';
    while (1) {
        // receive data
        int num_bytes = socket_obj.recv(buff, buff_size, 0);
        if (numBytes < 0) {
            std::cout << "error with recv errno is " << strerror(errno) << std::endl;
            return 5;
        }
        buff[(numBytes + 1) % buf_size] = '\0';

        std::cout << "got msg:" << buff << std::endl;

        if (strcmp(buff, "end") == 0) {
            break;
        }
    }

    return 0;
}
