#pragma once
#include "Verbose.hpp"
#include <memory>
#include <type_traits>

// Derived publicly from std::enable_shared_from_this
class AbstractWidget : public std::enable_shared_from_this<AbstractWidget> {
public:
    // Non-member factory function.
    template <typename Derived, typename... Args>
    friend std::shared_ptr<Derived> create(Args&&... args);

    std::shared_ptr<AbstractWidget> clone() const;

    virtual ~AbstractWidget() = default;

protected:
    struct Protected; // Tag type.

    AbstractWidget(const std::string& name);
    AbstractWidget(const AbstractWidget&) = default;

    // Rule of five.
    AbstractWidget() = delete;
    AbstractWidget(AbstractWidget&&) = delete;
    AbstractWidget& operator=(const AbstractWidget&) = delete;
    AbstractWidget& operator=(AbstractWidget&&) = delete;

private:
    virtual std::shared_ptr<AbstractWidget> cloneImpl() const = 0;

    Verbose v_;
};

struct AbstractWidget::Protected {
    // Explicit constructor prevents brace initialization.
    explicit Protected() = default;
};

// Non-member factory function.
template <typename Derived, typename... Args>
std::shared_ptr<Derived> create(Args&&... args) {
    static_assert(
            not (std::is_base_of_v<AbstractWidget, std::remove_reference_t<Args>>
                 || ...),
            "Copy construction is forbidden!");
    return std::make_shared<Derived>(
            AbstractWidget::Protected {}, std::forward<Args>(args)...);
}
