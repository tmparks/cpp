#pragma once

// Emulate C++17 language features.

// NOLINTBEGIN(*-macro-usage)

#if __cplusplus < 201703L

#define STRUCTURED_BINDING_1(a, tuple) auto&& a = std::get<0>(tuple);

#define STRUCTURED_BINDING_2(a, b, tuple) \
    auto&& b = std::get<1>(tuple); \
    STRUCTURED_BINDING_1(a, tuple)

#define STRUCTURED_BINDING_3(a, b, c, tuple) \
    auto&& c = std::get<2>(tuple); \
    STRUCTURED_BINDING_2(a, b, tuple);

#define STRUCTURED_BINDING_4(a, b, c, d, tuple) \
    auto&& d = std::get<3>(tuple); \
    STRUCTURED_BINDING_3(a, b, c, tuple);

#define STRUCTURED_BINDING_5(a, b, c, d, e, tuple) \
    auto&& e = std::get<4>(tuple); \
    STRUCTURED_BINDING_4(a, b, c, d, tuple);

#define STRUCTURED_BINDING_6(a, b, c, d, e, f, tuple) \
    auto&& f = std::get<5>(tuple); \
    STRUCTURED_BINDING_5(a, b, c, d, e, tuple);

#else // C++17

#define STRUCTURED_BINDING_1(a, tuple) auto&& [a] = tuple;

#define STRUCTURED_BINDING_2(a, b, tuple) auto&& [a, b] = tuple;

#define STRUCTURED_BINDING_3(a, b, c, tuple) auto&& [a, b, c] = tuple;

#define STRUCTURED_BINDING_4(a, b, c, d, tuple) auto&& [a, b, c, d] = tuple;

#define STRUCTURED_BINDING_5(a, b, c, d, e, tuple) \
    auto&& [a, b, c, d, e] = tuple;

#define STRUCTURED_BINDING_6(a, b, c, d, e, f, tuple) \
    auto&& [a, b, c, d, e, f] = tuple;

#endif // C++17

// NOLINTEND(*-macro-usage)
