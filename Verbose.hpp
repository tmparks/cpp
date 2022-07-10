#pragma once
#include <string>

class Verbose
{
protected:
    Verbose() = delete;                        // no default constructor
    explicit Verbose(const std::string& name); // constructor
    Verbose(const Verbose& other);             // copy constructor
    Verbose(Verbose&& other);                  // move constructor
    Verbose& operator=(const Verbose& other);  // copy assignment
    Verbose& operator=(Verbose&& other);       // move assignment
    virtual ~Verbose();                        // destructor

    std::string name_ { }; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};