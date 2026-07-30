#pragma once
#include <cstddef>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

template<typename T>
class Allocator;

template<typename T, typename U>
constexpr bool operator==(const Allocator<T>& lhs,
                          const Allocator<U>& rhs) noexcept
{
    return true;
}

template<typename T, typename U>
constexpr bool operator!=(const Allocator<T>& lhs,
                          const Allocator<U>& rhs)
{
    return !(lhs == rhs);
}

template<typename T>
class Allocator
{
public:
    using value_type = T;

    template<typename U>
    struct rebind {
        using other = Allocator<U>;
    };

    using is_always_equal = std::false_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

public:
    Allocator() = default;
    Allocator(const Allocator<T>& rhs) = default;
    Allocator<T>& operator=(const Allocator<T>& rhs) = default;
    Allocator(Allocator<T>&& rhs) = default;
    Allocator<T>& operator=(Allocator<T>&& rhs) = default;
    ~Allocator() noexcept = default;

    template<typename U>
    constexpr Allocator(const Allocator<U>& rhs) noexcept {}

    [[nodiscard]] constexpr T* allocate(std::size_t count) const;
    constexpr void deallocate(T* ptr, std::size_t count) const noexcept;

    template<typename... Args>
    constexpr void construct(T* ptr, Args&&... args) const;
    constexpr void destroy(T* ptr) const noexcept;
    [[nodiscard]] constexpr std::size_t max_size() const noexcept;
};

template<typename T>
constexpr T* Allocator<T>::allocate(std::size_t count) const {
    if (count == 0) {
        return nullptr;
    }

    if (count > max_size()) {
        throw std::bad_array_new_length();
    }

    return static_cast<T*>(::operator new(sizeof(T) * count));
}

template<typename T>
constexpr void Allocator<T>::deallocate(T* ptr, std::size_t count) const noexcept {
    if (ptr != nullptr) {
        ::operator delete(ptr, (sizeof(T) * count));
    }
}

template<typename T>
template<typename... Args>
constexpr void Allocator<T>::construct(T* ptr, Args&&... args) const {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}

template<typename T>
constexpr void Allocator<T>::destroy(T* ptr) const noexcept {
    if (ptr != nullptr) {
        ptr->~T();
    }
}

template<typename T>
constexpr std::size_t Allocator<T>::max_size() const noexcept {
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}
