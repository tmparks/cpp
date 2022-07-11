#pragma once
#include <string>

class Verbose
{
public:
    virtual ~Verbose();                        // destructor

protected:
    Verbose() = delete;                        // no default constructor
    explicit Verbose(const std::string& name); // constructor
    Verbose(const Verbose& other);             // copy constructor
    Verbose(Verbose&& other);                  // move constructor
    Verbose& operator=(const Verbose& other);  // copy assignment
    Verbose& operator=(Verbose&& other);       // move assignment

    std::string name_ { }; // NOLINT(*-non-private-member-variables-in-classes)
};