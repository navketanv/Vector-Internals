#pragma once
#include "Allocator.h"
#include <cstddef>
#include <type_traits>

template<typename T, typename Alloc = Allocator<T>>
class BufferStorage
{
private:
    static constexpr bool kStorageSwapNoexcept = std::is_nothrow_swappable_v<Alloc> &&
                                                 std::is_nothrow_swappable_v<T*> &&
                                                 std::is_nothrow_swappable_v<std::size_t>;
public:
    BufferStorage() = default;
    explicit BufferStorage(std::size_t capacity);

    BufferStorage(const BufferStorage<T, Alloc>& rhs) = delete;
    BufferStorage<T, Alloc>& operator=(const BufferStorage<T, Alloc>& rhs) = delete;

    BufferStorage(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>);
    BufferStorage<T, Alloc>& operator=(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>);

    ~BufferStorage();

    void swap(BufferStorage<T, Alloc>& rhs) noexcept(kStorageSwapNoexcept);

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

template<typename T, typename Alloc>
void swap(BufferStorage<T, Alloc>& lhs,
          BufferStorage<T, Alloc>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

#include "BufferStorage.tpp"