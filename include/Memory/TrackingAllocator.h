#pragma once
#include <cstddef>
#include <memory>
#include <limits>
#include <new>
#include <utility>
#include "AllocationRegistry.h"

template<typename T>
class TrackingAllocator;

template<typename T1, typename T2>
constexpr bool operator==(const TrackingAllocator<T1>&,
                          const TrackingAllocator<T2>&) noexcept;

template<typename T1, typename T2>
constexpr bool operator!=(const TrackingAllocator<T1>&,
                          const TrackingAllocator<T2>&) noexcept;

template<typename T>
class TrackingAllocator
{
public:
    using value_type = T;

    template<typename U>
    struct rebind {
        using other = TrackingAllocator<U>;
    };

    using is_always_equal = std::false_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

    TrackingAllocator(const TrackingAllocator<T>&) = default;
    TrackingAllocator<T>& operator=(const TrackingAllocator<T>&) = default;
    TrackingAllocator(TrackingAllocator&&) noexcept = default;
    TrackingAllocator<T>& operator=(TrackingAllocator<T>&&) noexcept = default;
    ~TrackingAllocator() noexcept = default;

    explicit TrackingAllocator(std::size_t id, std::shared_ptr<AllocationRegistry> pRegistry) noexcept;

    template<typename U>
    TrackingAllocator(const TrackingAllocator<U>& rhs) noexcept;

    [[nodiscard]] constexpr T* allocate(std::size_t count) const;
    constexpr void deallocate(T* ptr, std::size_t count) const noexcept;

    template<typename... Args>
    constexpr void construct(T* ptr, Args&&... args) const;
    constexpr void destroy(T* ptr) const noexcept;

    [[nodiscard]] constexpr std::size_t id() const noexcept;
    [[nodiscard]] constexpr std::size_t max_size() const noexcept;

private:
    std::size_t m_id{};
    std::shared_ptr<AllocationRegistry> m_pRegistry;
};

template<typename T>
TrackingAllocator<T>::TrackingAllocator(std::size_t id, std::shared_ptr<AllocationRegistry> pRegistry) noexcept
    : m_id(id)
    , m_pRegistry(std::move(pRegistry)) {}

template<typename T>
template<typename U>
TrackingAllocator<T>::TrackingAllocator(const TrackingAllocator<U>& rhs) noexcept
    : m_id(rhs.m_id)
    , m_pRegistry(rhs.m_pRegistry) {}

template<typename T>
constexpr T* TrackingAllocator<T>::allocate(std::size_t count) const {
    if (count == 0) {
        return nullptr;
    }

    if (count > max_size()) {
        throw std::bad_array_new_length();
    }

    T* ptr = static_cast<T*>(::operator new(count * sizeof(T)));
    if (m_pRegistry != nullptr) {
        AllocationStats& stats = m_pRegistry->statsFor(m_id);
        ++stats.allocations;
    }
    return ptr;
}

template<typename T>
constexpr void TrackingAllocator<T>::deallocate(T* ptr, std::size_t count) const noexcept {
    if (ptr != nullptr) {
        ::operator delete(ptr, (count * sizeof(T)));

        if (m_pRegistry != nullptr) {
            if (AllocationStats* pStats = m_pRegistry->findStats(m_id)) {
                ++pStats->deallocations;
            }
        }
    }
}

template<typename T>
template<typename... Args>
constexpr void TrackingAllocator<T>::construct(T* ptr, Args&&... args) const {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);

        if (m_pRegistry != nullptr) {
            AllocationStats& stats = m_pRegistry->statsFor(m_id);
            ++stats.constructions;
        }
    }
}

template<typename T>
constexpr void TrackingAllocator<T>::destroy(T* ptr) const noexcept {
    if (ptr != nullptr) {
        ptr->~T();

        if (m_pRegistry != nullptr) {
            if (AllocationStats* pStats = m_pRegistry->findStats(m_id)) {
                ++pStats->destructions;
            }
        }
    }
}

template<typename T>
constexpr std::size_t TrackingAllocator<T>::id() const noexcept {
    return m_id;
}

template<typename T>
constexpr std::size_t TrackingAllocator<T>::max_size() const noexcept {
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}

template<typename T1, typename T2>
constexpr bool operator==(const TrackingAllocator<T1>& lhs,
                     const TrackingAllocator<T2>& rhs) noexcept
{
    return (lhs.id() == rhs.id());
}

template<typename T1, typename T2>
constexpr bool operator!=(const TrackingAllocator<T1>& lhs,
                          const TrackingAllocator<T2>& rhs) noexcept
{
    return !(lhs == rhs);
}
