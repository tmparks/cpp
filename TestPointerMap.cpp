#include "PointerMap.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    // Print the elements of a PointerMap.
    void print([[maybe_unused]] const RawPointerMap<std::string, Verbose<>>& container) {
        for (const auto& elem : container) {
            std::cout << gsl::czstring{__func__} << ": key=" << elem.first
                      << " value=" << elem.second << std::endl;
            const auto& name = elem.second.name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }

#if __cplusplus >= 202002L // since C++20

    using std::erase_if;

#else // until C++20

    template <typename M, typename Predicate>
    typename M::size_type erase_if(M& m, Predicate p) {
        auto size = m.size();
        for (auto i = m.begin(); i != m.end();) {
            if (p(*i))
                i = m.erase(i);
            else
                ++i;
        }
        return size - m.size();
    }

#endif // C++20

} // anonymous namespace

TEST(SharedPointerMap, emplace) {
    // Place elements into container as pointers.
    auto container = SharedPointerMap<std::string, Verbose<>>{};
    auto& base = container.base;
    base.emplace("one", std::make_shared<Verbose<>>("one"));
    base.emplace("two", std::make_shared<Verbose<>>("two"));
    base.emplace("three", std::make_shared<Verbose<>>("three"));

    print(reference(container));
}

TEST(SharedPointerMap, insert) {
    // Place elements into container as pointers.
    auto container = SharedPointerMap<std::string, Verbose<>>{};
    auto& base = container.base;
    base.insert({"one", std::make_shared<Verbose<>>("one")});
    base.insert({"two", std::make_shared<Verbose<>>("two")});
    base.insert({"three", std::make_shared<Verbose<>>("three")});

    print(reference(container));
}

TEST(SharedPointerMap, shallow_copy) {
    // Place elements into container as pointers.
    auto container = SharedPointerMap<std::string, Verbose<>>{};
    auto& base = container.base;
    base.emplace("one", std::make_shared<Verbose<>>("one"));
    base.emplace("two", std::make_shared<Verbose<>>("two"));
    base.emplace("three", std::make_shared<Verbose<>>("three"));

    // Copying a container of pointers does not copy the referenced objects.
    auto container_copy = container;
    print(reference(container_copy));

    container_copy["one"].name() = "alfa";
    container_copy["two"].name() = "bravo";
    container_copy["three"].name() = "charlie";

    print(reference(container));

    EXPECT_EQ(container, container_copy); // shallow copy
    EXPECT_NE("one", container["one"].name());
    EXPECT_NE("two", container["two"].name());
}

TEST(SharedPointerMap, erase_if) {
    auto container = SharedPointerMap<std::string, Verbose<>>{};
    auto& base = container.base;
    base.emplace("one", std::make_shared<Verbose<>>("one"));
    base.emplace("two", std::make_shared<Verbose<>>("two"));
    base.emplace("three", std::make_shared<Verbose<>>("three"));

    // In order to avoid copying/moving values,
    // use the base vector of pointers for erase-remove.
    auto predicate = [](const auto& elem) {
        return elem.second->name() == "two";
    };
    EXPECT_EQ(1, erase_if(base, predicate));
    EXPECT_EQ(2, container.size());

    print(reference(container));
}

TEST(UniquePointerMap, uncopyable) {
    auto container = UniquePointerMap<std::string, Verbose<>>{};
    auto& base = container.base;
    base.emplace("one", std::make_unique<Verbose<>>("one"));
    base.emplace("two", std::make_unique<Verbose<>>("two"));
    base.emplace("three", std::make_unique<Verbose<>>("three"));

    // auto container_copy = container; // compile time error
}
