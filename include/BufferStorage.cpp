#include "BufferStorage.h"
#include <utility>

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(std::size_t capacity)
    : m_allocator{}
    , m_pData(capacity ? m_allocator.allocate(capacity) : nullptr)
    , m_capacity(capacity) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>)
    : m_allocator(std::move(rhs.m_allocator))
    , m_pData(std::exchange(rhs.m_pData, nullptr))
    , m_capacity(std::exchange(rhs.m_capacity, 0)) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>& BufferStorage<T, Alloc>::operator=(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>) {
    if (this != &rhs) {
        m_allocator.deallocate(m_pData, m_capacity);
        m_allocator = std::move(rhs.m_allocator);
        m_pData = std::exchange(rhs.m_pData, nullptr);
        m_capacity = std::exchange(rhs.m_capacity, 0);
    }
    return *this;
}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::~BufferStorage<T, Alloc>() {
    m_allocator.deallocate(m_pData, m_capacity);
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::swap(BufferStorage<T, Alloc>& rhs) noexcept(kStorageSwapNoexcept) {
    using std::swap;
    // allocator and storage must remain paired
    swap(m_allocator, rhs.m_allocator);
    swap(m_pData, rhs.m_pData);
    swap(m_capacity, rhs.m_capacity);
}

template<typename T, typename Alloc>
T* BufferStorage<T, Alloc>::data() noexcept {
    return m_pData;
}

template<typename T, typename Alloc>
const T* BufferStorage<T, Alloc>::data() const noexcept {
    return m_pData;
}

template<typename T, typename Alloc>
Alloc& BufferStorage<T, Alloc>::allocator() noexcept {
    return m_allocator;
}

template<typename T, typename Alloc>
const Alloc& BufferStorage<T, Alloc>::allocator() const noexcept {
    return m_allocator;
}

template<typename T, typename Alloc>
std::size_t BufferStorage<T, Alloc>::capacity() const noexcept {
    return m_capacity;
}

template<typename T, typename Alloc>
bool BufferStorage<T, Alloc>::empty() const noexcept {
    return m_capacity == 0;
}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::operator bool() const noexcept {
    return m_pData != nullptr;
}
