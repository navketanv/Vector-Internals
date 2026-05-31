#include "Vector.h"
#include <stdexcept>
#include <utility>

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector()
    : m_storage{}
    , m_size{} {}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::size_t size, const T& value)
    : m_storage(size)
    , m_size{}
{
    try {
        for (std::size_t index = 0; index < size; ++index) {
            allocator().construct(data() + index, value);
            ++m_size;
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::size_t size)
    : Vector(size, T{}) {}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<T> list)
    : m_storage(list.size())
    , m_size{}
{
    try {
        for (const auto& value : list) {
            allocator().construct(data() + m_size, value);
            ++m_size;
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& rhs)
    : m_storage(rhs.capacity())
    , m_size{}
{
    try {
        for (std::size_t index = 0; index < rhs.size(); ++index) {
            allocator().construct(data() + index, rhs[index]);
            ++m_size;
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<BufferStorage<T, Alloc>>)
    : m_storage(std::move(rhs.m_storage))
    , m_size(std::exchange(rhs.m_size, 0)) {}

template<typename T, typename Alloc>
Vector<T, Alloc>::~Vector() noexcept {
    clear();
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::clear() noexcept {
    while (m_size > 0) {
        --m_size;
        allocator().destroy(data() + m_size);
    }
}

template<typename T, typename Alloc>
std::size_t Vector<T, Alloc>::size() const noexcept {
    return m_size;
}

template<typename T, typename Alloc>
std::size_t Vector<T, Alloc>::capacity() const noexcept {
    return m_storage.capacity();
}

template<typename T, typename Alloc>
bool Vector<T, Alloc>::empty() const noexcept {
    return m_size == 0;
}

template<typename T, typename Alloc>
T* Vector<T, Alloc>::data() noexcept {
    return m_storage.data();
}

template<typename T, typename Alloc>
const T* Vector<T, Alloc>::data() const noexcept {
    return m_storage.data();
}

template<typename T, typename Alloc>
Alloc& Vector<T, Alloc>::allocator() noexcept {
    return m_storage.allocator();
}

template<typename T, typename Alloc>
const Alloc& Vector<T, Alloc>::allocator() const noexcept {
    return m_storage.allocator();
}

template<typename T, typename Alloc>
T& Vector<T, Alloc>::operator[](std::size_t index) noexcept {
    return *(data() + index);
}

template<typename T, typename Alloc>
const T& Vector<T, Alloc>::operator[](std::size_t index) const noexcept {
    return *(data() + index);
}

template<typename T, typename Alloc>
T& Vector<T, Alloc>::at(std::size_t index) {
    if (index < m_size) {
        return *(data() + index);
    }
    throw std::out_of_range("Vector::at");
}

template<typename T, typename Alloc>
const T& Vector<T, Alloc>::at(std::size_t index) const {
    if (index < m_size) {
        return *(data() + index);
    }
    throw std::out_of_range("Vector::at");
}

template<typename T, typename Alloc>
T& Vector<T, Alloc>::front() noexcept {
    return *data();
}

template<typename T, typename Alloc>
const T& Vector<T, Alloc>::front() const noexcept {
    return *data();
}

template<typename T, typename Alloc>
T& Vector<T, Alloc>::back() noexcept {
    return *(data() + (m_size - 1));
}

template<typename T, typename Alloc>
const T& Vector<T, Alloc>::back() const noexcept {
    return *(data() + (m_size - 1));
}
