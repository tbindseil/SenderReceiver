#pragma once

#include <Environment.h>

class SinglePacketEnvironment : SinglePacketEnvironment {
public:
    SinglePacketEnvironment();
    ~SinglePacketEnvironment();

private:
    void doInit() override;
};
