#pragma once
#include "Allocator.h"
#include <cstddef>
#include <type_traits>

template<typename T, typename Alloc = Allocator<T>>
class BufferStorage
{
public:
    using allocator_type = Alloc;
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

    BufferStorage(const BufferStorage<T, Alloc>& rhs) = delete;
    BufferStorage<T, Alloc>& operator=(const BufferStorage<T, Alloc>& rhs) = delete;

    BufferStorage(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>);
    BufferStorage<T, Alloc>& operator=(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>);

    ~BufferStorage();

    void swap(BufferStorage<T, Alloc>& rhs) noexcept(kStorageSwapNoexcept);

    [[nodiscard]] typename BufferStorage<T, Alloc>::pointer data() noexcept;
    [[nodiscard]] typename BufferStorage<T, Alloc>::const_pointer data() const noexcept;

    typename BufferStorage<T, Alloc>::allocator_type& allocator() noexcept;
    const typename BufferStorage<T, Alloc>::allocator_type& allocator() const noexcept;

    [[nodiscard]] typename BufferStorage<T, Alloc>::size_type capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

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

#include "BufferStorage.tpp"