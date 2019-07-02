#include <memory>
#include <iostream>
#include <fstream>
#include <utils.h>

constexpr uint16_t MaxLineLength = 512;

void ExecuteInstruction(const std::string& curr);

/**
 * @brief starts program by reading filename as argv[1]
 */
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: a.out [test_instruction_file]" << std::endl;
        return 1;
    }

    std::ifstream instructionFile();
    instructionFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        instructionFile.open(argv[1]);
        std::string curr;
        do
        {
            curr.resize(MaxLineLength);
            instructionsFile.getline(curr);
            ExecuteInstruction(curr);
        } while (!instructionFile.eof());
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

void ExecuteInstruction(const std::string& curr)
{
    size_t spacePos = 0;
    size_t spacePos = curr.find(spacePos, " ");
    std::string cmd = curr.substr(0, spacePos);

    std::vector<std::string> args;
    while (spacePos != std::string::npos)
    {
        size_t nextSpacePos = curr.find(spacePos, " ");
        args.emplace_back(curr.substr(spacePos, nextSpacePos));
        spacePos = nextSpacePos;
    }

    if (cmd.compare("listen") == 0)
    {
        if (args.size() < 1)
        {
            std::cerr << "listen cmd requires ip addr to listen on" << std::endl;
            return;
        }

        const std::string& ipAddr = args.at(0);

        // at this point, it looks like we'll want an listener object to create
        // how to tell that it listens? ...
    }
    else
    {
        std::cerr << "invalid cmd " << cmd << std::endl;
    }
}
