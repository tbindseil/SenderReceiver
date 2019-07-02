#pragma once

class Environment {
public:
    static CreateEnvironment(const std::string& arg);
    Environment();
    ~Environment();

    void init();

private:
    virtual void doInit() = 0;
};
