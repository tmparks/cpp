#include <string>

class Verbose
{
public:
    Verbose();                                // default constructor
    Verbose(const Verbose& other);            // copy constructor
    Verbose(Verbose&& other);                 // move constructor
    Verbose& operator=(const Verbose& other); // copy assignment
    Verbose& operator=(Verbose&& other);      // move assignment
    virtual ~Verbose();                       // destructor

protected:
    Verbose(std::string&& name);
    std::string name_ { "anonymous" }; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};