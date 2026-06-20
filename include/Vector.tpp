#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cstdint>

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
    reallocateStorage(newCapacity);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(Vector<T, Alloc>::const_iterator pos, const T& value) {
    // iterator validation for raw-pointer iterators
    if (!isValidIterator(pos)) {
        throw std::out_of_range("Vector::insert invalid iterator");
    }
    const std::size_t insertionIndex = static_cast<std::size_t>(std::distance(cbegin(), pos));
    if (m_size < capacity()) {
        return insertInPlace(insertionIndex, value);
    }
    return reallocateAndInsert(insertionIndex, value);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(Vector<T, Alloc>::const_iterator pos, T&& value) {
    // iterator validation for raw-pointer iterators
    if (!isValidIterator(pos)) {
        throw std::out_of_range("Vector::insert invalid iterator");
    }
    const std::size_t insertionIndex = static_cast<std::size_t>(std::distance(cbegin(), pos));
    if (m_size < capacity()) {
        return insertInPlace(insertionIndex, std::move(value));
    }
    return reallocateAndInsert(insertionIndex, std::move(value));
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insert(Vector<T, Alloc>::const_iterator pos, size_type count, const T& value) {
    if (!isValidIterator(pos)) {
        throw std::out_of_range("Vector::insert invalid iterator");
    }
    const std::size_t insertionIndex = static_cast<std::size_t>(std::distance(cbegin(), pos));
    if (count == 0) {
        return data() + insertionIndex;
    }
    checkGrowth(count);
    if (m_size + count <= capacity()) {
        return insertInPlace(insertionIndex, count, value);
    }
    return reallocateAndInsert(insertionIndex, count, value);
}

template<typename T, typename Alloc>
template<typename InputIt>
    requires(
        !std::is_integral_v<InputIt>)
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(Vector<T, Alloc>::const_iterator pos, InputIt first, InputIt last) {
    if (!isValidIterator(pos)) {
        throw std::out_of_range("Vector::insert invalid iterator");
    }

    using Category = typename std::iterator_traits<InputIt>::iterator_category;
    return insertRange(pos, first, last, Category{});
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(Vector<T, Alloc>::const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::erase(Vector<T, Alloc>::const_iterator first, Vector<T, Alloc>::const_iterator last) {
    if (!isValidIterator(first) ||
        !isValidIterator(last) ||
        (first > last))
    {
        throw std::out_of_range("Vector::erase invalid iterator range");
    }

    const std::size_t eraseStartIndex = static_cast<std::size_t>(std::distance(cbegin(), first));
    const std::size_t eraseCount = static_cast<std::size_t>(std::distance(first, last));
    if (eraseCount == 0) {
        return (data() + eraseStartIndex);
    }
    for (std::size_t index = eraseStartIndex; index + eraseCount < m_size; ++index) {
        data()[index] = std::move(data()[index + eraseCount]);
    }

    for (std::size_t index = 0; index < eraseCount; ++index) {
        --m_size;
        allocator().destroy(data() + m_size);
    }
    return (data() + eraseStartIndex);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::erase(Vector<T, Alloc>::const_iterator pos) {
    return erase(pos, pos + 1);
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
void Vector<T, Alloc>::shrink_to_fit() {
    if (m_size == capacity()) {
        return;
    }
    reallocateStorage(m_size);
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector<T, Alloc>& rhs) noexcept(kVectorSwapNoexcept) {
    using std::swap;
    swap(m_storage, rhs.m_storage);
    swap(m_size, rhs.m_size);
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
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::begin() noexcept {
    return data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::begin() const noexcept {
    return data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cbegin() const noexcept {
    return data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::end() noexcept {
    return (data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::end() const noexcept {
    return (data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cend() const noexcept {
    return (data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rbegin() noexcept {
    return Vector<T, Alloc>::reverse_iterator(end());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::rbegin() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(end());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crbegin() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(cend());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rend() noexcept {
    return Vector<T, Alloc>::reverse_iterator(begin());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::rend() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(begin());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crend() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(cbegin());
}

template<typename T, typename Alloc>
template<typename InputIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRange(Vector<T, Alloc>::const_iterator pos, InputIt first, InputIt last, std::input_iterator_tag) {
    Vector<T, Alloc> temp;
    for (InputIt it = first; it != last; ++it) {
        temp.push_back(*it);
    }
    return insert(pos, temp.begin(), temp.end());
}

template<typename T, typename Alloc>
template<typename ForwardIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRange(Vector<T, Alloc>::const_iterator pos, ForwardIt first, ForwardIt last, std::forward_iterator_tag) {
    const std::size_t insertionIndex = static_cast<std::size_t>(std::distance(cbegin(), pos));
    const std::size_t count = static_cast<std::size_t>(std::distance(first, last));
    if (count == 0) {
        return data() + insertionIndex;
    }
    checkGrowth(count);
    if (m_size + count <= capacity()) {
        return insertRangeInPlace(insertionIndex, count, first, last);
    }
    return reallocateAndInsertRange(insertionIndex, count, first, last);
}

template<typename T, typename Alloc>
template<typename BidirectionalIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRange(Vector<T, Alloc>::const_iterator pos, BidirectionalIt first, BidirectionalIt last, std::bidirectional_iterator_tag) {
    return insertRange(pos, first, last, std::forward_iterator_tag{});
}

template<typename T, typename Alloc>
template<typename RandomAccessIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRange(Vector<T, Alloc>::const_iterator pos, RandomAccessIt first, RandomAccessIt last, std::random_access_iterator_tag) {
    return insertRange(pos, first, last, std::forward_iterator_tag{});
}

template<typename T, typename Alloc>
template<typename ContiguousIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRange(Vector<T, Alloc>::const_iterator pos, ContiguousIt first, ContiguousIt last, std::contiguous_iterator_tag) {
    return insertRange(pos, first, last, std::forward_iterator_tag{});
}

// TODO:
// During reallocation consider std::move_if_noexcept
// to better match std::vector strong exception guarantees.
template<typename T, typename Alloc>
void Vector<T, Alloc>::reallocateStorage(std::size_t newCapacity) {
    if (newCapacity < m_size) {
        throw std::length_error("reallocateStorage() capacity is smaller than m_size");
    }

    BufferStorage<T, Alloc> newStorage(newCapacity);
    std::size_t alreadyConstructed{};
    try {
        for (std::size_t index = 0; index < m_size; ++index) {
            newStorage.allocator().construct(newStorage.data() + index, data()[index]);
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const std::size_t oldSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = oldSize;
}

template<typename T, typename Alloc>
template<typename... Args>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertInPlace(std::size_t insertionIndex, Args&&... args) {
    if (empty() || (insertionIndex == m_size)) {
        allocator().construct(data() + insertionIndex, std::forward<Args>(args)...);
    } else {
        T temp(std::forward<Args>(args)...);
        allocator().construct(data() + m_size, std::move(data()[m_size - 1]));
        for (std::size_t index = m_size - 1; index > insertionIndex; --index) {
            data()[index] = std::move(data()[index - 1]);
        }
        data()[insertionIndex] = std::move(temp);
    }
    ++m_size;
    return (data() + insertionIndex);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertInPlace(std::size_t insertionIndex, std::size_t count, const T& value) {
    if (insertionIndex == m_size) {
        // covers empty vector case as well as insertion at the end, when no shifting of data is required.
        for (std::size_t index = 0; index < count; ++index) {
            allocator().construct(data() + m_size + index, value);
        }
    } else {
        const std::size_t suffixSize = (m_size - insertionIndex);
        if (count <= suffixSize) {
            for (std::size_t index = 0; index < count; ++index) {
                allocator().construct(data() + m_size + index, std::move(data()[m_size - count + index]));
            }
            for (std::size_t index = m_size - count; index > insertionIndex; --index) {
                data()[index + count - 1] = std::move(data()[index - 1]);
            }
            for (std::size_t index = 0; index < count; ++index) {
                data()[insertionIndex + index] = value;
            }
        } else {
            const std::size_t overflow = count - suffixSize;
            for (std::size_t index = 0; index < overflow; ++index) {
                allocator().construct(data() + m_size + index, value);
            }
            for (std::size_t index = overflow; index < count; ++index) {
                allocator().construct(data() + m_size + index, std::move(data()[m_size + index - count]));
            }
            for (std::size_t index = insertionIndex; index < m_size; ++index) {
                data()[index] = value;
            }
        }
    }
    m_size += count;
    return (data() + insertionIndex);
}

template<typename T, typename Alloc>
template<typename ForwardIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRangeInPlace(std::size_t insertionIndex, std::size_t count, ForwardIt first, ForwardIt last) {
    Vector<T, Alloc> temp;
    for (ForwardIt it = first; it != last; ++it) {
        temp.push_back(*it);
    }
    if (insertionIndex == m_size) {
        for (std::size_t index = 0; index < count; ++index) {
            allocator().construct(data() + m_size + index, std::move(temp[index]));
        }
    } else {
        const std::size_t suffixSize = m_size - insertionIndex;
        if (count <= suffixSize) {
            for (std::size_t index = 0; index < count; ++index) {
                allocator().construct(data() + m_size + index, std::move(data()[m_size + index - count]));
            }
            for (std::size_t index = m_size - count; index > insertionIndex; --index) {
                data()[index + count - 1] = std::move(data()[index - 1]);
            }
            for (std::size_t index = 0; index < count; ++index) {
                data()[insertionIndex + index] = std::move(temp[index]);
            }
        } else {
            const std::size_t overflow = count - suffixSize;
            for (std::size_t suffixIndex = 0; suffixIndex < suffixSize; ++suffixIndex) {
                allocator().construct(data() + m_size + overflow + suffixIndex, std::move(data()[insertionIndex + suffixIndex]));
            }
            std::size_t tempIndex{};
            for (std::size_t index = insertionIndex; index < m_size && tempIndex < count; ++index, ++tempIndex) {
                data()[index] = std::move(temp[tempIndex]);
            }
            for (std::size_t index = 0; index < overflow && tempIndex < count; ++index, ++tempIndex) {
                allocator().construct(data() + m_size + index, std::move(temp[tempIndex]));
            }
        }
    }
    m_size += count;
    return (data() + insertionIndex);
}

// TODO:
// During reallocation consider std::move_if_noexcept
// to better match std::vector strong exception guarantees.
template<typename T, typename Alloc>
template<typename... Args>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::reallocateAndInsert(std::size_t insertionIndex, Args&&... args) {
    BufferStorage<T, Alloc> newStorage(nextCapacity(m_size + 1));
    std::size_t alreadyConstructed{};
    try {
        for (std::size_t index = 0; index < insertionIndex; ++index) {
            newStorage.allocator().construct(newStorage.data() + index, data()[index]);
            ++alreadyConstructed;
        }
        newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::forward<Args>(args)...);
        ++alreadyConstructed;
        for (std::size_t index = insertionIndex; index < m_size; ++index) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, data()[index]);
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const std::size_t newSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = newSize;
    return (data() + insertionIndex);
}
// TODO:
// During reallocation consider std::move_if_noexcept
// to better match std::vector strong exception guarantees.
template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::reallocateAndInsert(std::size_t insertionIndex, std::size_t count, const T& value) {
    BufferStorage<T, Alloc> newStorage(nextCapacity(m_size + count));
    std::size_t alreadyConstructed{};
    try {
        for (std::size_t index = 0; index < insertionIndex; ++index) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, data()[index]);
            ++alreadyConstructed;
        }
        for (std::size_t index = 0; index < count; ++index) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, value);
            ++alreadyConstructed;
        }
        for (std::size_t index = insertionIndex; index < m_size; ++index) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, data()[index]);
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const std::size_t newSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = newSize;
    return (data() + insertionIndex);
}

// TODO:
// During reallocation consider std::move_if_noexcept
// to better match std::vector strong exception guarantees.
template<typename T, typename Alloc>
template<typename ForwardIt>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::reallocateAndInsertRange(std::size_t insertionIndex, std::size_t count, ForwardIt first, ForwardIt last) {
    BufferStorage<T, Alloc> newStorage(nextCapacity(m_size + count));
    std::size_t alreadyConstructed{};
    try {
        for (std::size_t index = 0; index < insertionIndex; ++index) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, data()[index]);
            ++alreadyConstructed;
        }
        for (ForwardIt it = first; it != last; ++it) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, *it);
            ++alreadyConstructed;
        }
        for (std::size_t index = insertionIndex; index < m_size; ++index) {
            newStorage.allocator().construct(newStorage.data() + alreadyConstructed, data()[index]);
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const std::size_t newSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = newSize;
    return (data() + insertionIndex);
}

template<typename T, typename Alloc>
bool Vector<T, Alloc>::isValidIterator(typename Vector<T, Alloc>::const_iterator pos) const {
    if (empty()) {
        if (pos != cbegin()) {
            return false;
        }
    } else {
        const auto beginAddr = reinterpret_cast<std::uintptr_t>(data());
        const auto endAddr = reinterpret_cast<std::uintptr_t>(data() + m_size);
        const auto posAddr = reinterpret_cast<std::uintptr_t>(pos);
        if (posAddr < beginAddr || posAddr > endAddr) {
            return false;
        }
    }
    return true;
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

template<typename T, typename Alloc>
void Vector<T, Alloc>::checkGrowth(size_type count) const {
    if (count > maxSize() - m_size)
    {
        throw std::length_error("Vector growth would exceed maxSize()");
    }
}
