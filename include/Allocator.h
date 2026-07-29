#pragma once
#include <cstddef>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

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
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

    Allocator() = default;
    template<typename U>
    Allocator(const Allocator<U>& rhs) noexcept {}

    [[nodiscard]] T* allocate(std::size_t count) const;
    void deallocate(T* ptr, std::size_t count) const noexcept;

    template<typename... Args>
    void construct(T* ptr, Args&&... args) const;
    void destroy(T* ptr) const noexcept;
    std::size_t max_size() const noexcept;
};

template<typename T>
T* Allocator<T>::allocate(std::size_t count) const {
    if (count == 0) {
        return nullptr;
    }

    if (count > Allocator<T>::max_size()) {
        throw std::bad_array_new_length();
    }

    return static_cast<T*>(::operator new(sizeof(T) * count));
}

template<typename T>
void Allocator<T>::deallocate(T* ptr, std::size_t count) const noexcept {
    if (ptr != nullptr) {
        ::operator delete(ptr, (sizeof(T) * count));
        ptr = nullptr;
    }
}

template<typename T>
template<typename... Args>
void Allocator<T>::construct(T* ptr, Args&&... args) const {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}

template<typename T>
void Allocator<T>::destroy(T* ptr) const noexcept {
    if (ptr != nullptr) {
        ptr->~T();
    }
}

template<typename T>
std::size_t Allocator<T>::max_size() const noexcept {
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}
