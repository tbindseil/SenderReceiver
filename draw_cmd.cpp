#include <draw_cmd.h>

#include <sys/types.h>

draw_cmd::draw_cmd(uint32_t type, uint32_t radius, uint32_t x, uint32_t y, uint32_t color) :
    _type(type), _radius(radius), _x(x), _y(y), _color(color)
{
}

void draw_cmd::hton()
{
    _type_n = htonl(_type);
    _radius_n = htonl(_radius);
    _x_n = htonl(_x);
    _y_n = htonl(_y);
    _color_n = htonl(_color);
}

uint8_t* draw_cmd::get_network_buff_start()
{
    return (uint8_t*)(&(this->_type_n));
}
