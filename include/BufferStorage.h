#pragma once
#include "Allocator.h"
#include <type_traits>

template<typename T, typename Alloc = Allocator<T>>
class BufferStorage
{
public:
    BufferStorage() = default;
    explicit BufferStorage(std::size_t capacity);

    BufferStorage(const BufferStorage& rhs) = delete;
    BufferStorage& operator=(const BufferStorage& rhs) = delete;

    BufferStorage(BufferStorage&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>);
    BufferStorage& operator=(BufferStorage&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>);

    ~BufferStorage();

    [[nodiscard]] T* data() noexcept;
    [[nodiscard]] const T* data() const noexcept;

    Alloc& allocator() noexcept;
    const Alloc& allocator() const noexcept;

    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

private:
    Alloc m_allocator{};
    T* m_pData{nullptr};
    std::size_t m_capacity{};
};
