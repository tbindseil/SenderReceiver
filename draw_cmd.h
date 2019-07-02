#pragma once

#include <netdb.h>

class draw_cmd {
public:
    draw_cmd(uint32_t type, uint32_t radius, uint32_t x, uint32_t y, uint32_t color);

    /**
     * @breif populate the network byte ordered member variables
     */
    void hton();

    uint8_t* get_network_buff_start();

    static constexpr size_t needed_buff_size = 20;

private:
    uint32_t _type;
    uint32_t _radius;
    uint32_t _x;
    uint32_t _y;
    uint32_t _color;

    uint32_t _type_n;
    uint32_t _radius_n;
    uint32_t _x_n;
    uint32_t _y_n;
    uint32_t _color_n;
};
