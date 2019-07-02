#include <Environment.h>
#include <SinglePacketEnvironment.h>

// static
std::unique_ptr<Environment> Environment::CreateEnvironment(const std::string& arg)
{
    if (arg.compare("ip2ip") == 0)
    {
        return std::make_unique<SinglePacketEnvironment>();
    }
    else
    {
        std::cerr << "Invalid environment arg " << arg << std::endl;
        assert(false);
    }
}

Environment::Environment()
{
}

Environment::~Environment()
{
}

void Environment::init()
{
    doInit();
}
