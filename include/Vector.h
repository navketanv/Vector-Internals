#pragma once
#include "Allocator.h"
#include "BufferStorage.h"
#include <initializer_list>

template<typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    Vector();
    Vector(std::size_t size, const T& value);
    explicit Vector(std::size_t size);
    Vector(std::initializer_list<T> list);
    Vector(const Vector& rhs);
    Vector(Vector&& rhs) noexcept(std::is_nothrow_move_constructible_v<BufferStorage<T, Alloc>>);
    ~Vector() noexcept;

    void clear() noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] T* data() noexcept;
    [[nodiscard]] const T* data() const noexcept;

    Alloc& allocator() noexcept;
    const Alloc& allocator() const noexcept;

    [[nodiscard]] T& operator[](std::size_t index) noexcept;
    [[nodiscard]] const T& operator[](std::size_t index) const noexcept;

    [[nodiscard]] T& at(std::size_t index);
    [[nodiscard]] const T& at(std::size_t index) const;

    [[nodiscard]] T& front() noexcept;
    [[nodiscard]] const T& front() const noexcept;

    [[nodiscard]] T& back() noexcept;
    [[nodiscard]] const T& back() const noexcept;

private:
    BufferStorage<T, Alloc> m_storage{};
    std::size_t m_size{};
};
