#pragma once
#include "Allocator.h"

template<typename T, typename Alloc = Allocator<T>>
class BufferStorage
{
public:
    BufferStorage() = default;
    explicit BufferStorage(std::size_t capacity);

    BufferStorage(const BufferStorage& rhs) = delete;
    BufferStorage& operator=(const BufferStorage& rhs) = delete;

    BufferStorage(BufferStorage&& rhs) noexcept;
    BufferStorage& operator=(BufferStorage&& rhs) noexcept;

    ~BufferStorage();

private:
    Alloc m_allocator{};
    T* m_pData{nullptr};
    std::size_t m_capacity{};
};
