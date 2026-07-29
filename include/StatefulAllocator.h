#pragma once
#include <cstddef>
#include <limits>
#include <new>

template<typename T>
class StatefulAllocator;

template<typename T>
bool operator==(const StatefulAllocator<T>& lhs, const StatefulAllocator<T>& rhs) noexcept;

template<typename T>
bool operator!=(const StatefulAllocator<T>& lhs, const StatefulAllocator<T>& rhs) noexcept;

template<typename T>
class StatefulAllocator
{
public:
    using value_type = T;
    using is_always_equal = std::false_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

public:
    StatefulAllocator() = default;
    explicit StatefulAllocator(std::size_t id);

    StatefulAllocator(const StatefulAllocator<T>& rhs) = default;
    StatefulAllocator<T>& operator=(const StatefulAllocator<T>& rhs) = default;

    StatefulAllocator(StatefulAllocator<T>&& rhs) = default;
    StatefulAllocator<T>& operator=(StatefulAllocator<T>&& rhs) = default;

    ~StatefulAllocator() = default;

    [[nodiscard]] T* allocate(std::size_t count) const;
    void deallocate(T* ptr, std::size_t count) const noexcept;

    template<typename... Args>
    void construct(T* ptr, Args&&... args) const;
    void destroy(T* ptr) const noexcept;
    [[nodiscard]] std::size_t max_size() const noexcept;

private:
    std::size_t m_id{};

    friend bool operator== <> (const StatefulAllocator<T>& lhs, const StatefulAllocator<T>& rhs) noexcept;
    friend bool operator!= <> (const StatefulAllocator<T>& lhs, const StatefulAllocator<T>& rhs) noexcept;
};

template<typename T>
StatefulAllocator<T>::StatefulAllocator(std::size_t id)
    : m_id(id) {}
template<typename T>
T* StatefulAllocator<T>::allocate(std::size_t count) const {
    if (count == 0) {
        return nullptr;
    }

    if (count > StatefulAllocator<T>::max_size()) {
        throw std::bad_array_new_length();
    }

    return static_cast<T*>(::operator new(count * sizeof(T)));
}

template<typename T>
void StatefulAllocator<T>::deallocate(T* ptr, std::size_t count) const noexcept {
    if (ptr != nullptr) {
        ::operator delete(ptr, (count * sizeof(T)));
        ptr = nullptr;
    }
}

template<typename T>
template<typename... Args>
void StatefulAllocator<T>::construct(T* ptr, Args&&... args) const {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}

template<typename T>
void StatefulAllocator<T>::destroy(T* ptr) const noexcept {
    if (ptr != nullptr) {
        ptr->~T();
    }
}

template<typename T>
std::size_t StatefulAllocator<T>::max_size() const noexcept {
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}

template<typename T>
bool operator==(const StatefulAllocator<T>& lhs, const StatefulAllocator<T> &rhs) noexcept {
    return (lhs.m_id == rhs.m_id);
}

template<typename T>
bool operator!=(const StatefulAllocator<T>& lhs, const StatefulAllocator<T>& rhs) noexcept {
    return (lhs.m_id != rhs.m_id);
}
