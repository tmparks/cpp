#include "Array.hpp"

// An array that can grow.
class StretchableArray : public Array
{
public:
    StretchableArray(gsl::index size);

protected:
    void CheckBounds(gsl::index i) override;
    void Resize(gsl::index size);
};
