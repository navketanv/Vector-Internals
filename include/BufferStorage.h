#pragma once
#include "Allocator.h"
#include "AllocatorPolicy.h"

template<typename T, typename Alloc = Allocator<T>>
class BufferStorage
{
public:
    using value_type = T;
    using allocator_type = Alloc;
    using AllocatorTraits = std::allocator_traits<Alloc>;
    using AllocPolicy = AllocatorPolicy<Alloc>;
    using size_type = std::size_t;
    using pointer = T*;
    using const_pointer = const T*;

private:
    static constexpr bool kStorageSwapNoexcept = std::is_nothrow_swappable_v<Alloc> &&
                                                 std::is_nothrow_swappable_v<T*> &&
                                                 std::is_nothrow_swappable_v<size_type>;
public:
    BufferStorage() = default;
    explicit BufferStorage(BufferStorage<T, Alloc>::size_type capacity);
    explicit BufferStorage(const BufferStorage<T, Alloc>::allocator_type& allocator, BufferStorage<T, Alloc>::size_type capacity);

    BufferStorage(const BufferStorage<T, Alloc>& rhs) = delete;
    BufferStorage<T, Alloc>& operator=(const BufferStorage<T, Alloc>& rhs) = delete;

    BufferStorage(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>);
    BufferStorage<T, Alloc>& operator=(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>);

    ~BufferStorage() noexcept;

    void swap(BufferStorage<T, Alloc>& rhs) noexcept(kStorageSwapNoexcept);
    // Preconditions:
    // - The current storage has already been released.
    // - The caller guarantees that adopting rhs's storage is allocator-safe.
    void stealStorage(BufferStorage<T, Alloc>& rhs) noexcept;

    [[nodiscard]] typename BufferStorage<T, Alloc>::pointer data() noexcept;
    [[nodiscard]] typename BufferStorage<T, Alloc>::const_pointer data() const noexcept;

    typename BufferStorage<T, Alloc>::allocator_type& allocator() noexcept;
    const typename BufferStorage<T, Alloc>::allocator_type& allocator() const noexcept;

    [[nodiscard]] typename BufferStorage<T, Alloc>::size_type capacity() const noexcept;
    [[nodiscard]] bool hasStorage() const noexcept;

private:
    void releaseStorage() noexcept;

private:
    Alloc m_allocator{};
    T* m_pData{nullptr};
    size_type m_capacity{};
};

template<typename T, typename Alloc>
void swap(BufferStorage<T, Alloc>& lhs,
          BufferStorage<T, Alloc>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(BufferStorage<T, Alloc>::size_type capacity)
    : m_allocator{}
    , m_pData(capacity ? BufferStorage<T, Alloc>::AllocatorTraits::allocate(m_allocator, capacity) : nullptr)
    , m_capacity(capacity) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(const BufferStorage<T, Alloc>::allocator_type& allocator, BufferStorage<T, Alloc>::size_type capacity)
    : m_allocator(allocator)
    , m_pData(capacity ? BufferStorage<T, Alloc>::AllocatorTraits::allocate(m_allocator, capacity) : nullptr)
    , m_capacity(capacity) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>)
    : m_allocator(std::move(rhs.m_allocator))
    , m_pData(std::exchange(rhs.m_pData, nullptr))
    , m_capacity(std::exchange(rhs.m_capacity, 0)) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>& BufferStorage<T, Alloc>::operator=(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>) {
    if (this != &rhs) {
        releaseStorage();
        m_allocator = std::move(rhs.m_allocator);
        m_pData = std::exchange(rhs.m_pData, nullptr);
        m_capacity = std::exchange(rhs.m_capacity, 0);
    }
    return *this;
}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::~BufferStorage() noexcept {
    releaseStorage();
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::swap(BufferStorage<T, Alloc>& rhs) noexcept(kStorageSwapNoexcept) {
    using std::swap;
    swap(m_allocator, rhs.m_allocator);
    swap(m_pData, rhs.m_pData);
    swap(m_capacity, rhs.m_capacity);
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::stealStorage(BufferStorage<T, Alloc>& rhs) noexcept {
    releaseStorage();
    m_pData = std::exchange(rhs.m_pData, nullptr);
    m_capacity = std::exchange(rhs.m_capacity, 0);
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::pointer
BufferStorage<T, Alloc>::data() noexcept {
    return m_pData;
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::const_pointer
BufferStorage<T, Alloc>::data() const noexcept {
    return m_pData;
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::allocator_type&
BufferStorage<T, Alloc>::allocator() noexcept {
    return m_allocator;
}

template<typename T, typename Alloc>
const typename BufferStorage<T, Alloc>::allocator_type&
BufferStorage<T, Alloc>::allocator() const noexcept {
    return m_allocator;
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::size_type
BufferStorage<T, Alloc>::capacity() const noexcept {
    return m_capacity;
}

template<typename T, typename Alloc>
bool BufferStorage<T, Alloc>::hasStorage() const noexcept {
    return (m_pData != nullptr);
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::releaseStorage() noexcept {
    if (m_pData != nullptr) {
        BufferStorage<T, Alloc>::AllocatorTraits::deallocate(m_allocator, m_pData, m_capacity);
        m_pData = nullptr;
        m_capacity = 0;
    }
}
