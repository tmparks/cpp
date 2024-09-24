#pragma once
#include <string>

class Verbose {
public:
    Verbose() noexcept = delete;                   // no default constructor
    Verbose(std::string name);                     // constructor
    Verbose(const Verbose& other);                 // copy constructor
    Verbose(Verbose&& other) noexcept;             // move constructor
    Verbose& operator=(const Verbose& other);      // copy assignment
    Verbose& operator=(Verbose&& other) noexcept;  // move assignment
    virtual ~Verbose() noexcept;                   // destructor
    friend void swap(Verbose&, Verbose&) noexcept; // non-member swap

    std::string& name() noexcept;
    [[nodiscard]] const std::string& name() const noexcept;

private:
    std::string name_ {};
};

std::ostream& operator<<(std::ostream& stream, const Verbose& object);
