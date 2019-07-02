#include <utils.h>
#include <cerror>

void utils::printError(const std::string& tag)
{
    std::cerr << tag << std::endl;
    std::cerr << std::strerr(errno) << std::endl;
}
