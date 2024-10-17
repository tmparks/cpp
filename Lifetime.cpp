template <typename T> class Lref {
public:
    T& value;
};

template <typename T> class Cref {
public:
    const T& value;
};

template <typename T> class RRef {
public:
    T&& value;
};

