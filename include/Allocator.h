#pragma once
#include <cstddef>

template<typename T>
class Allocator
{
public:
    using value_type = T;

    [[nodiscard]] T* allocate(std::size_t count);
    void deallocate(T* ptr, std::size_t count) noexcept;

    template<typename... Args>
    void construct(T* ptr, Args&&... args);
    void destroy(T* ptr) noexcept;
};
