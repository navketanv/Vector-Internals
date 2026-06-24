#pragma once
#include <cstddef>
#include <utility>
#include <type_traits>

template<typename T>
class Allocator;

template<typename T, typename U>
bool operator==(const Allocator<T>& lhs, const Allocator<U>& rhs) noexcept {
    return true;
}

template<typename T, typename U>
bool operator!=(const Allocator<T>& lhs, const Allocator<U>& rhs) {
    return false;
}

template<typename T>
class Allocator
{
public:
    using value_type = T;
    using is_always_equal = std::true_type;

    Allocator() = default;
    template<typename U>
    Allocator(const Allocator<U>& rhs) noexcept {}

    [[nodiscard]] T* allocate(std::size_t count);
    void deallocate(T* ptr, std::size_t count) noexcept;

    template<typename... Args>
    void construct(T* ptr, Args&&... args);
    void destroy(T* ptr) noexcept;
    std::size_t max_size() const noexcept;
};

#include "Allocator.tpp"