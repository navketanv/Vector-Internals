#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cstdint>

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>::allocator_type& alloc, const Vector<T, Alloc>& rhs)
    : m_storage(alloc, 0)
    , m_size{}
{
    assignFromRange(alloc, rhs.begin(), rhs.end());
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>&& rhs)
    : m_storage(alloc, 0)
    , m_size{}
{
    if (Vector<T, Alloc>::AllocPolicy::allocators_compatible(alloc, rhs.allocator())) {
        m_storage.stealStorage(rhs.m_storage);
        m_size = std::exchange(rhs.m_size, 0);
    } else {
        assignFromRange(alloc, std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
        rhs.clear();
    }
}

template<typename T, typename Alloc>
template<std::forward_iterator ForwardIt>
void Vector<T, Alloc>::assignFromRange(const Vector<T, Alloc>::allocator_type& alloc, ForwardIt first, ForwardIt last) {
    const size_type count = static_cast<size_type>(std::distance(first, last));
    BufferStorage<T, Alloc> newStorage(alloc, count);
    size_type alreadyConstructed{};
    try {
        for (ForwardIt itr = first; itr != last; ++itr) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, *itr);
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            Vector<T, Alloc>::AllocatorTraits::destroy(newStorage.allocator(), newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    clear();
    m_storage = std::move(newStorage);
    m_size = alreadyConstructed;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector()
    : m_storage{}
    , m_size{} {}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>::size_type size, const T& value)
    : m_storage(size)
    , m_size{}
{
    try {
        for (size_type index = 0; index < size; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + index, value);
            //allocator().construct(data() + index, value);
            ++m_size;
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>::size_type size)
    : Vector(size, T{}) {}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<T> list)
    : m_storage(list.size())
    , m_size{}
{
    try {
        for (const auto& value : list) {
            Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, value);
            //allocator().construct(data() + m_size, value);
            ++m_size;
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, typename Alloc>
template<typename InputIt>
    requires(!std::is_integral_v<InputIt>)
Vector<T, Alloc>::Vector(InputIt first, InputIt last)
    : m_storage{}
    , m_size{}
{
    insert(end(), first, last);
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& rhs)
    : Vector<T, Alloc>(Vector<T, Alloc>::AllocPolicy::select_copy_constructor_allocator(rhs.allocator()), rhs)
{}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<BufferStorage<T, Alloc>>)
    : m_storage(std::move(rhs.m_storage))
    , m_size(std::exchange(rhs.m_size, 0)) {}

template<typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector<T, Alloc>& rhs)
{
    if (this != &rhs) {
        if constexpr (Vector<T, Alloc>::AllocPolicy::copy_assign_propagates) {
            Vector<T, Alloc> temp(rhs.allocator(), rhs);
            swap(temp);
            return *this;
        }
        if (rhs.size() <= capacity()) {
            const size_type common = std::min(rhs.size(), m_size);
            for (size_type index = 0; index < common; ++index) {
                data()[index] = rhs.data()[index];
            }
            if (rhs.size() <= m_size) {
                while (m_size > rhs.size()) {
                    --m_size;
                    //allocator().destroy(data() + m_size);
                    Vector<T, Alloc>::AllocatorTraits::destroy(allocator(), data() + m_size);
                }
            } else {
                while (m_size < rhs.size()) {
                    Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, rhs.data()[m_size]);
                    //allocator().construct(data() + m_size, rhs.data()[m_size]);
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
        if constexpr (Vector<T, Alloc>::AllocPolicy::move_assign_propagates) {
            clear();
            m_storage = std::move(rhs.m_storage);
            m_size = std::exchange(rhs.m_size, 0);
        } else {
            Vector<T, Alloc> temp(allocator(), std::move(rhs));
            swap(temp);
        }
    }
    return *this;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::~Vector() noexcept {
    clear();
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(Vector<T, Alloc>::size_type newCapacity) {
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
    const size_type insertionIndex = static_cast<size_type>(std::distance(cbegin(), pos));
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
    const size_type insertionIndex = static_cast<size_type>(std::distance(cbegin(), pos));
    if (m_size < capacity()) {
        return insertInPlace(insertionIndex, std::move(value));
    }
    return reallocateAndInsert(insertionIndex, std::move(value));
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insert(Vector<T, Alloc>::const_iterator pos, Vector<T, Alloc>::size_type count, const T& value) {
    if (!isValidIterator(pos)) {
        throw std::out_of_range("Vector::insert invalid iterator");
    }
    const size_type insertionIndex = static_cast<size_type>(std::distance(cbegin(), pos));
    if (count == 0) {
        return Vector<T, Alloc>::iterator(data() + insertionIndex);
    }
    checkGrowth(count);
    if (m_size + count <= capacity()) {
        return insertInPlace(insertionIndex, count, value);
    }
    return reallocateAndInsert(insertionIndex, count, value);
}

template<typename T, typename Alloc>
template<typename InputIt>
    requires(!std::is_integral_v<InputIt>)
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

    const size_type eraseStartIndex = static_cast<size_type>(std::distance(cbegin(), first));
    const size_type eraseCount = static_cast<size_type>(std::distance(first, last));
    if (eraseCount == 0) {
        return Vector<T, Alloc>::iterator(data() + eraseStartIndex);
    }
    for (size_type index = eraseStartIndex; index + eraseCount < m_size; ++index) {
        data()[index] = std::move(data()[index + eraseCount]);
    }

    for (size_type index = 0; index < eraseCount; ++index) {
        --m_size;
        //allocator().destroy(data() + m_size);
        Vector<T, Alloc>::AllocatorTraits::destroy(allocator(), data() + m_size);
    }
    return Vector<T, Alloc>::iterator(data() + eraseStartIndex);
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
    Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, value);
    //allocator().construct(data() + m_size, value);
    ++m_size;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(T&& value) {
    if (m_size == capacity()) {
        reserve(nextCapacity(m_size + 1));
    }
    Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, std::move(value));
    //allocator().construct(data() + m_size, std::move(value));
    ++m_size;
}

template<typename T, typename Alloc>
template<typename... Args>
typename Vector<T, Alloc>::reference
Vector<T, Alloc>::emplace_back(Args&&... args) {
    if (m_size == capacity()) {
        reserve(nextCapacity(m_size + 1));
    }
    Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, std::forward<Args>(args)...);
    //allocator().construct(data() + m_size, std::forward<Args>(args)...);
    ++m_size;
    return data()[m_size - 1];
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::pop_back() {
    if (m_size > 0) {
        --m_size;
        //allocator().destroy(data() + m_size);
        Vector<T, Alloc>::AllocatorTraits::destroy(allocator(), data() + m_size);
    }
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(Vector<T, Alloc>::size_type count, const T& value) {
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
            Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, value);
            //allocator().construct(data() + m_size, value);
            ++m_size;
        }
    }
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::resize(Vector<T, Alloc>::size_type count) {
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
        //allocator().destroy(data() + m_size);
        Vector<T, Alloc>::AllocatorTraits::destroy(allocator(), data() + m_size);
    }
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::size_type
Vector<T, Alloc>::size() const noexcept {
    return m_size;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::size_type
Vector<T, Alloc>::capacity() const noexcept {
    return m_storage.capacity();
}

template<typename T, typename Alloc>
bool Vector<T, Alloc>::empty() const noexcept {
    return m_size == 0;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::pointer
Vector<T, Alloc>::data() noexcept {
    return m_storage.data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_pointer
Vector<T, Alloc>::data() const noexcept {
    return m_storage.data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::allocator_type Vector<T, Alloc>::get_allocator() noexcept {
    return allocator();
}

template<typename T, typename Alloc>
const typename Vector<T, Alloc>::allocator_type Vector<T, Alloc>::get_allocator() const noexcept {
    return allocator();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference
Vector<T, Alloc>::operator[](size_type index) noexcept {
    return *(data() + index);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference
Vector<T, Alloc>::operator[](size_type index) const noexcept {
    return *(data() + index);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference
Vector<T, Alloc>::at(size_type index) {
    if (index < m_size) {
        return *(data() + index);
    }
    throw std::out_of_range("Vector::at");
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference
Vector<T, Alloc>::at(size_type index) const {
    if (index < m_size) {
        return *(data() + index);
    }
    throw std::out_of_range("Vector::at");
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference
Vector<T, Alloc>::front() noexcept {
    return *data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference
Vector<T, Alloc>::front() const noexcept {
    return *data();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference
Vector<T, Alloc>::back() noexcept {
    return *(data() + (m_size - 1));
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference
Vector<T, Alloc>::back() const noexcept {
    return *(data() + (m_size - 1));
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::begin() noexcept {
    return Vector<T, Alloc>::iterator(data());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator
Vector<T, Alloc>::begin() const noexcept {
    return Vector<T, Alloc>::const_iterator(data());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator
Vector<T, Alloc>::cbegin() const noexcept {
    return Vector<T, Alloc>::const_iterator(data());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::end() noexcept {
    return Vector<T, Alloc>::iterator(data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator
Vector<T, Alloc>::end() const noexcept {
    return Vector<T, Alloc>::const_iterator(data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_iterator
Vector<T, Alloc>::cend() const noexcept {
    return Vector<T, Alloc>::const_iterator(data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator
Vector<T, Alloc>::rbegin() noexcept {
    return Vector<T, Alloc>::reverse_iterator(end());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator
Vector<T, Alloc>::rbegin() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(end());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator
Vector<T, Alloc>::crbegin() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(cend());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reverse_iterator
Vector<T, Alloc>::rend() noexcept {
    return Vector<T, Alloc>::reverse_iterator(begin());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator
Vector<T, Alloc>::rend() const noexcept {
    return Vector<T, Alloc>::const_reverse_iterator(begin());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reverse_iterator
Vector<T, Alloc>::crend() const noexcept {
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
    const size_type insertionIndex = static_cast<size_type>(std::distance(cbegin(), pos));
    const size_type count = static_cast<size_type>(std::distance(first, last));
    if (count == 0) {
        return Vector<T, Alloc>::iterator(data() + insertionIndex);
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

template<typename T, typename Alloc>
void Vector<T, Alloc>::reallocateStorage(Vector<T, Alloc>::size_type newCapacity) {
    if (newCapacity < m_size) {
        throw std::length_error("reallocateStorage() capacity is smaller than m_size");
    }

    BufferStorage<T, Alloc> newStorage(newCapacity);
    size_type alreadyConstructed{};
    try {
        for (size_type index = 0; index < m_size; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + index, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + index, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            //newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
            Vector<T, Alloc>::AllocatorTraits::destroy(newStorage.allocator(), newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const size_type oldSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = oldSize;
}

template<typename T, typename Alloc>
template<typename... Args>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertInPlace(Vector<T, Alloc>::size_type insertionIndex, Args&&... args) {
    if (empty() || (insertionIndex == m_size)) {
        Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + insertionIndex, std::forward<Args>(args)...);
        //allocator().construct(data() + insertionIndex, std::forward<Args>(args)...);
    } else {
        T temp(std::forward<Args>(args)...);
        Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size, std::move(data()[m_size - 1]));
        //allocator().construct(data() + m_size, std::move(data()[m_size - 1]));
        for (size_type index = m_size - 1; index > insertionIndex; --index) {
            data()[index] = std::move(data()[index - 1]);
        }
        data()[insertionIndex] = std::move(temp);
    }
    ++m_size;
    return Vector<T, Alloc>::iterator(data() + insertionIndex);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertInPlace(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, const T& value) {
    if (insertionIndex == m_size) {
        // covers empty vector case as well as insertion at the end, when no shifting of data is required.
        for (size_type index = 0; index < count; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, value);
            //allocator().construct(data() + m_size + index, value);
        }
    } else {
        const size_type suffixSize = (m_size - insertionIndex);
        if (count <= suffixSize) {
            for (size_type index = 0; index < count; ++index) {
                Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, std::move(data()[m_size - count + index]));
                //allocator().construct(data() + m_size + index, std::move(data()[m_size - count + index]));
            }
            for (size_type index = m_size - count; index > insertionIndex; --index) {
                data()[index + count - 1] = std::move(data()[index - 1]);
            }
            for (size_type index = 0; index < count; ++index) {
                data()[insertionIndex + index] = value;
            }
        } else {
            const size_type overflow = count - suffixSize;
            for (size_type index = 0; index < overflow; ++index) {
                Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, value);
                //allocator().construct(data() + m_size + index, value);
            }
            for (size_type index = overflow; index < count; ++index) {
                Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, std::move(data()[m_size + index - count]));
                //allocator().construct(data() + m_size + index, std::move(data()[m_size + index - count]));
            }
            for (size_type index = insertionIndex; index < m_size; ++index) {
                data()[index] = value;
            }
        }
    }
    m_size += count;
    return Vector<T, Alloc>::iterator(data() + insertionIndex);
}

template<typename T, typename Alloc>
template<typename ForwardIt>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::insertRangeInPlace(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, ForwardIt first, ForwardIt last) {
    Vector<T, Alloc> temp;
    for (ForwardIt it = first; it != last; ++it) {
        temp.push_back(*it);
    }
    if (insertionIndex == m_size) {
        for (size_type index = 0; index < count; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, std::move(temp[index]));
            //allocator().construct(data() + m_size + index, std::move(temp[index]));
        }
    } else {
        const size_type suffixSize = m_size - insertionIndex;
        if (count <= suffixSize) {
            for (size_type index = 0; index < count; ++index) {
                Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, std::move(data()[m_size + index - count]));
                //allocator().construct(data() + m_size + index, std::move(data()[m_size + index - count]));
            }
            for (size_type index = m_size - count; index > insertionIndex; --index) {
                data()[index + count - 1] = std::move(data()[index - 1]);
            }
            for (size_type index = 0; index < count; ++index) {
                data()[insertionIndex + index] = std::move(temp[index]);
            }
        } else {
            const size_type overflow = count - suffixSize;
            for (size_type suffixIndex = 0; suffixIndex < suffixSize; ++suffixIndex) {
                Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + overflow + suffixIndex, std::move(data()[insertionIndex + suffixIndex]));
                //allocator().construct(data() + m_size + overflow + suffixIndex, std::move(data()[insertionIndex + suffixIndex]));
            }
            size_type tempIndex{};
            for (size_type index = insertionIndex; index < m_size && tempIndex < count; ++index, ++tempIndex) {
                data()[index] = std::move(temp[tempIndex]);
            }
            for (size_type index = 0; index < overflow && tempIndex < count; ++index, ++tempIndex) {
                Vector<T, Alloc>::AllocatorTraits::construct(allocator(), data() + m_size + index, std::move(temp[tempIndex]));
                //allocator().construct(data() + m_size + index, std::move(temp[tempIndex]));
            }
        }
    }
    m_size += count;
    return Vector<T, Alloc>::iterator(data() + insertionIndex);
}

template<typename T, typename Alloc>
template<typename... Args>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::reallocateAndInsert(Vector<T, Alloc>::size_type insertionIndex, Args&&... args) {
    BufferStorage<T, Alloc> newStorage(nextCapacity(m_size + 1));
    size_type alreadyConstructed{};
    try {
        for (size_type index = 0; index < insertionIndex; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + index, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + index, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
        Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, std::forward<Args>(args)...);
        //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::forward<Args>(args)...);
        ++alreadyConstructed;
        for (size_type index = insertionIndex; index < m_size; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            //newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
            Vector<T, Alloc>::AllocatorTraits::destroy(newStorage.allocator(), newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const size_type newSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = newSize;
    return Vector<T, Alloc>::iterator(data() + insertionIndex);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator
Vector<T, Alloc>::reallocateAndInsert(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, const T& value) {
    BufferStorage<T, Alloc> newStorage(nextCapacity(m_size + count));
    size_type alreadyConstructed{};
    try {
        for (size_type index = 0; index < insertionIndex; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
        for (size_type index = 0; index < count; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, value);
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, value);
            ++alreadyConstructed;
        }
        for (size_type index = insertionIndex; index < m_size; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            //newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
            Vector<T, Alloc>::AllocatorTraits::destroy(newStorage.allocator(), newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const size_type newSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = newSize;
    return Vector<T, Alloc>::iterator(data() + insertionIndex);
}

template<typename T, typename Alloc>
template<typename ForwardIt>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::reallocateAndInsertRange(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, ForwardIt first, ForwardIt last) {
    BufferStorage<T, Alloc> newStorage(nextCapacity(m_size + count));
    size_type alreadyConstructed{};
    try {
        for (size_type index = 0; index < insertionIndex; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
        for (ForwardIt it = first; it != last; ++it) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, *it);
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, *it);
            ++alreadyConstructed;
        }
        for (size_type index = insertionIndex; index < m_size; ++index) {
            Vector<T, Alloc>::AllocatorTraits::construct(newStorage.allocator(), newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            //newStorage.allocator().construct(newStorage.data() + alreadyConstructed, std::move_if_noexcept(data()[index]));
            ++alreadyConstructed;
        }
    } catch (...) {
        while (alreadyConstructed > 0) {
            --alreadyConstructed;
            Vector<T, Alloc>::AllocatorTraits::destroy(newStorage.allocator(), newStorage.data() + alreadyConstructed);
            //newStorage.allocator().destroy(newStorage.data() + alreadyConstructed);
        }
        throw;
    }
    const size_type newSize = alreadyConstructed;
    clear();
    m_storage = std::move(newStorage);
    m_size = newSize;
    return Vector<T, Alloc>::iterator(data() + insertionIndex);
}

template<typename T, typename Alloc>
bool Vector<T, Alloc>::isValidIterator(typename Vector<T, Alloc>::const_iterator pos) const {
    if (empty()) {
        return (pos == cbegin());
    }
    const auto ptr = pos.base();
    return ptr >= data() && ptr <= (data() + m_size);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::allocator_type&
Vector<T, Alloc>::allocator() noexcept {
    return m_storage.allocator();
}

template<typename T, typename Alloc>
const typename Vector<T, Alloc>::allocator_type&
Vector<T, Alloc>::allocator() const noexcept {
    return m_storage.allocator();
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::size_type
Vector<T, Alloc>::max_size() const noexcept {
    //return allocator().max_size();
    return Vector<T, Alloc>::AllocatorTraits::max_size(allocator());
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::size_type
Vector<T, Alloc>::nextCapacity(Vector<T, Alloc>::size_type required) const {
    const size_type possibleMaxSize = max_size();
    const size_type currentCapacity = capacity();

    if (required > possibleMaxSize) {
        throw std::length_error("Vector capacity exceeds max_size()");
    }

    size_type newCapacity = 0;
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
void Vector<T, Alloc>::checkGrowth(Vector<T, Alloc>::size_type count) const {
    if (count > max_size() - m_size)
    {
        throw std::length_error("Vector growth would exceed max_size()");
    }
}
