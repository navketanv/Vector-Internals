#include "Vector.h"
#include <algorithm>
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
Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector<T, Alloc>& rhs)
{
    if (this != &rhs) {
        if (rhs.size() <= capacity()) {
            const std::size_t common = std::min(rhs.size(), m_size);
            for (std::size_t index = 0; index < common; ++index) {
                data()[index] = rhs.data()[index];
            }
            if (rhs.size() <= m_size) {
                while (m_size > rhs.size()) {
                    --m_size;
                    allocator().destroy(data() + m_size);
                }
            } else {
                while (m_size < rhs.size()) {
                    allocator().construct(data() + m_size, rhs.data()[m_size]);
                    ++m_size;
                }
            }
        } else {
            Vector<T, Alloc> temp(rhs);
            *this = std::move(temp);
        }
    }
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<BufferStorage<T, Alloc>>) {
    if (this != &rhs) {
        clear();
        m_storage = std::move(rhs.m_storage);
        m_size = std::exchange(rhs.m_size, 0);
    }
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::~Vector() noexcept {
    clear();
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(std::size_t newCapacity) {
    if (newCapacity <= capacity()) {
        return;
    }

    BufferStorage<T, Alloc> newStorage(newCapacity);
    std::size_t alreadyConstructed{};
    try {
        while (alreadyConstructed < m_size) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, data()[alreadyConstructed]);
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
        }
        throw;
    }

    const std::size_t oldSize = m_size;
    clear();
    m_storage = std::move(newStorage);
    m_size = oldSize;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(const T& value) {
    if (m_size == capacity()) {
        reserve(nextCapacity(m_size + 1));
    }
    allocator().construct(data() + m_size, value);
    ++m_size;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(T&& value) {
    if (m_size == capacity()) {
        reserve(nextCapacity(m_size + 1));
    }
    allocator().construct(data() + m_size, std::move(value));
    ++m_size;
}

template<typename T, typename Alloc>
template<typename... Args>
T& Vector<T, Alloc>::emplace_back(Args&&... args) {
    if (m_size == capacity()) {
        reserve(nextCapacity(m_size + 1));
    }
    allocator().construct(data() + m_size, std::forward<Args>(args)...);
    ++m_size;
    return data()[m_size - 1];
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::pop_back() {
    if (m_size > 0) {
        --m_size;
        allocator().destroy(data() + m_size);
    }
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(std::size_t count, const T& value) {
    if (count == m_size) {
        return;
    }

    if (count < m_size) {
        while (count < m_size) {
            pop_back();
        }
    } else {
        if (count > capacity()) {
            reserve(nextCapacity(count));
        }
        while (count > m_size) {
            allocator().construct(data() + m_size, value);
            ++m_size;
        }
    }
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(std::size_t count) {
    resize(count, T{});
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

template<typename T, typename Alloc>
std::size_t Vector<T, Alloc>::maxSize() const noexcept {
    return allocator().maxSize();
}

template<typename T, typename Alloc>
std::size_t Vector<T, Alloc>::nextCapacity(std::size_t required) const {
    const std::size_t possibleMaxSize = maxSize();
    const std::size_t currentCapacity = capacity();

    if (required > possibleMaxSize) {
        throw std::length_error("Vector capacity exceeds maxSize()");
    }

    std::size_t newCapacity = 0;
    if (currentCapacity == 0) {
        newCapacity = 1;
    } else if (currentCapacity > possibleMaxSize / 2) {
        newCapacity = possibleMaxSize;
    } else {
        newCapacity = currentCapacity * 2;
    }

    newCapacity = std::min(newCapacity, possibleMaxSize);
    return std::max(newCapacity, required);
}
