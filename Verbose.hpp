#include <string>

class Verbose
{
public:
    virtual ~Verbose();                        // destructor

protected:
    explicit Verbose(const std::string& name); // constructor
    Verbose(const Verbose& other);             // copy constructor
    Verbose(Verbose&& other);                  // move constructor
    Verbose& operator=(const Verbose& other);  // copy assignment
    Verbose& operator=(Verbose&& other);       // move assignment

    std::string name_ { "anonymous" }; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};