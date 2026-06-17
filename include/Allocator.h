#pragma once
#include <cstddef>
#include <utility>

template<typename T>
class Allocator
{
public:
    using value_type = T;

    [[nodiscard]] T* allocate(std::size_t count);
    void deallocate(T* ptr, std::size_t count) noexcept;

    template<typename... Args>
    void construct(T* ptr, Args&&... args) {
        if (ptr != nullptr) {
            ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
        }
    }
    void destroy(T* ptr) noexcept;
    std::size_t maxSize() const noexcept;
};
