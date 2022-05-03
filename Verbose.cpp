#include "Verbose.hpp"
#include <iostream>
#include <utility>

Verbose::Verbose()
{
    std::cout << name_ << ": default constructor" << std::endl;
}

Verbose::Verbose(const Verbose& other) : name_(other.name_)
{
    std::cout << name_ << ": copy constructor" << std::endl;
}

Verbose::Verbose(Verbose&& other) : name_(other.name_)
{
    std::cout << name_ << ": move constructor" << std::endl;
}

Verbose& Verbose::operator=(const Verbose& other)
{
    name_ = other.name_;
    std::cout << name_ << ": copy assignment" << std::endl;
    return *this;
}

Verbose& Verbose::operator=(Verbose&& other)
{
    name_ = other.name_;
    std::cout << name_ << ": move assignment" << std::endl;
    return *this;
}

Verbose::~Verbose()
{
    std::cout << name_ << ": destructor" << std::endl;
}

Verbose::Verbose(std::string&& name) : name_(name)
{
    std::cout << name_ << ": constructor" << std::endl;
}
