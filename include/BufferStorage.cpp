#include "BufferStorage.h"

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(std::size_t capacity)
    : m_allocator{}
    , m_pData(capacity ? m_allocator.allocate(capacity) : nullptr)
    , m_capacity(capacity) {}

