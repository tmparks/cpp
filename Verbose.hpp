#pragma once
#include <string>

class Verbose
{
public:
    Verbose() = delete;                            // no default constructor
    explicit Verbose(const std::string& name);     // constructor
    Verbose(const Verbose& other);                 // copy constructor
    Verbose(Verbose&& other) noexcept;             // move constructor
    Verbose& operator=(const Verbose& other);      // copy assignment
    Verbose& operator=(Verbose&& other) noexcept;  // move assignment
    virtual ~Verbose();                            // destructor
    friend void swap(Verbose&, Verbose&) noexcept; // non-member swap

protected:
    std::string name_ { }; // NOLINT(*-non-private-member-variables-in-classes)
};