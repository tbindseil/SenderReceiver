#pragma once

#include <socket.h>

class listener {
public:
    listener(const socket& sock);
    ~listener();

private:
    _sock;
};
