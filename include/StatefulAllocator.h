#pragma once
#include <cstddef>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

template<typename T>
class StatefulAllocator;

template<typename T1, typename T2>
constexpr bool operator==(const StatefulAllocator<T1>& lhs,
                          const StatefulAllocator<T2>& rhs) noexcept;

template<typename T1, typename T2>
constexpr bool operator!=(const StatefulAllocator<T1>& lhs,
                          const StatefulAllocator<T2>& rhs) noexcept;

template<typename T>
class StatefulAllocator
{
public:
    using value_type = T;

    template<typename U>
    struct rebind
    {
        using other = StatefulAllocator<U>;
    };

    using is_always_equal = std::false_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

public:    
    StatefulAllocator() = default;
    StatefulAllocator(const StatefulAllocator<T>& rhs) = default;
    StatefulAllocator<T>& operator=(const StatefulAllocator<T>& rhs) = default;
    StatefulAllocator(StatefulAllocator<T>&& rhs) = default;
    StatefulAllocator<T>& operator=(StatefulAllocator<T>&& rhs) = default;
    ~StatefulAllocator() noexcept = default;

    explicit constexpr StatefulAllocator(std::size_t id) noexcept;

    template<typename U>
    constexpr StatefulAllocator(const StatefulAllocator<U>& rhs) noexcept;

    [[nodiscard]] constexpr T* allocate(std::size_t count) const;
    constexpr void deallocate(T* ptr, std::size_t count) const noexcept;

    template<typename... Args>
    constexpr void construct(T* ptr, Args&&... args) const;
    constexpr void destroy(T* ptr) const noexcept;
    [[nodiscard]] constexpr std::size_t id() const noexcept;
    [[nodiscard]] constexpr std::size_t max_size() const noexcept;

private:
    std::size_t m_id{};
};

template<typename T>
constexpr StatefulAllocator<T>::StatefulAllocator(std::size_t id) noexcept
    : m_id(id) {}

template<typename T>
template<typename U>
constexpr StatefulAllocator<T>::StatefulAllocator(const StatefulAllocator<U>& rhs) noexcept
    : m_id(rhs.id()) {}

template<typename T>
constexpr T* StatefulAllocator<T>::allocate(std::size_t count) const {
    if (count == 0) {
        return nullptr;
    }

    if (count > max_size()) {
        throw std::bad_array_new_length();
    }

    return static_cast<T*>(::operator new(count * sizeof(T)));
}

template<typename T>
constexpr void StatefulAllocator<T>::deallocate(T* ptr, std::size_t count) const noexcept {
    if (ptr != nullptr) {
        ::operator delete(ptr, (count * sizeof(T)));
    }
}

template<typename T>
template<typename... Args>
constexpr void StatefulAllocator<T>::construct(T* ptr, Args&&... args) const {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}

template<typename T>
constexpr void StatefulAllocator<T>::destroy(T* ptr) const noexcept {
    if (ptr != nullptr) {
        ptr->~T();
    }
}

template<typename T>
constexpr std::size_t StatefulAllocator<T>::id() const noexcept {
    return m_id;
}

template<typename T>
constexpr std::size_t StatefulAllocator<T>::max_size() const noexcept {
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}

template<typename T1, typename T2>
constexpr bool operator==(const StatefulAllocator<T1>& lhs,
                          const StatefulAllocator<T2>& rhs) noexcept
{
    return (lhs.id() == rhs.id());
}

template<typename T1, typename T2>
constexpr bool operator!=(const StatefulAllocator<T1>& lhs,
                          const StatefulAllocator<T2>& rhs) noexcept
{
    return !(lhs == rhs);
}
