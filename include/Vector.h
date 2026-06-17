#pragma once
#include "Allocator.h"
#include "BufferStorage.h"
#include <initializer_list>
#include <cstddef>
#include <type_traits>
#include <iterator>

template<typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    using value_type = T;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference = const T&;

    using pointer = T*;
    using const_pointer = const T*;

    using iterator = T*;
    using const_iterator = const T*;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
static constexpr bool kVectorSwapNoexcept = std::is_nothrow_swappable_v<BufferStorage<T, Alloc>> &&
                                            std::is_nothrow_swappable_v<std::size_t>;
public:
    Vector();
    Vector(std::size_t size, const T& value);
    explicit Vector(std::size_t size);
    Vector(std::initializer_list<T> list);
    Vector(const Vector<T, Alloc>& rhs);
    Vector(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<BufferStorage<T, Alloc>>);
    Vector<T, Alloc>& operator=(const Vector<T, Alloc>& rhs);
    Vector<T, Alloc>& operator=(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<BufferStorage<T, Alloc>>);
    ~Vector() noexcept;

    void reserve(std::size_t newCapacity);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, const T& value);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, T&& value);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, size_type count, const T& value);
    template<typename InputIt>
        requires(
            !std::is_integral_v<InputIt>
            )
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, InputIt first, InputIt last);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, std::initializer_list<T> ilist);

    void push_back(const T& value);
    void push_back(T&& value);

    template<typename... Args>
    T& emplace_back(Args&&... args);
    void pop_back();
    void resize(std::size_t count, const T& value);
    void resize(std::size_t count);
    void shrink_to_fit();
    void swap(Vector<T, Alloc>& rhs) noexcept(kVectorSwapNoexcept);
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

    [[nodiscard]] typename Vector<T, Alloc>::iterator begin() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_iterator begin() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_iterator cbegin() const noexcept;


    [[nodiscard]] typename Vector<T, Alloc>::iterator end() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_iterator end() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_iterator cend() const noexcept;

    [[nodiscard]] typename Vector<T, Alloc>::reverse_iterator rbegin() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reverse_iterator crbegin() const noexcept;


    [[nodiscard]] typename Vector<T, Alloc>::reverse_iterator rend() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reverse_iterator rend() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reverse_iterator crend() const noexcept;

private:
    template<typename InputIt>
    typename Vector<T, Alloc>::iterator insertRange(Vector<T, Alloc>::const_iterator pos, InputIt first, InputIt last, std::input_iterator_tag);
    template<typename ForwardIt>
    typename Vector<T, Alloc>::iterator insertRange(Vector<T, Alloc>::const_iterator pos, ForwardIt first, ForwardIt last, std::forward_iterator_tag);
    template<typename BidirectionalIt>
    typename Vector<T, Alloc>::iterator insertRange(Vector<T, Alloc>::const_iterator pos, BidirectionalIt first, BidirectionalIt last, std::bidirectional_iterator_tag);
    template<typename RandomAccessIt>
    typename Vector<T, Alloc>::iterator insertRange(Vector<T, Alloc>::const_iterator pos, RandomAccessIt first, RandomAccessIt last, std::random_access_iterator_tag);
    template<typename ContiguousIt>
    typename Vector<T, Alloc>::iterator insertRange(Vector<T, Alloc>::const_iterator pos, ContiguousIt first, ContiguousIt last, std::contiguous_iterator_tag);

    void reallocateStorage(std::size_t newCapacity);
    template<typename... Args>
    typename Vector<T, Alloc>::iterator insertInPlace(std::size_t insertionIndex, Args&&... args);
    typename Vector<T, Alloc>::iterator insertInPlace(std::size_t insertionIndex, std::size_t count, const T& value);
    template<typename ForwardIt>
    typename Vector<T, Alloc>::iterator insertRangeInPlace(std::size_t insertionIndex, std::size_t count, ForwardIt first, ForwardIt last);
    template<typename... Args>
    typename Vector<T, Alloc>::iterator reallocateAndInsert(std::size_t insertionIndex, Args&&... args);
    typename Vector<T, Alloc>::iterator reallocateAndInsert(std::size_t insertionIndex, std::size_t count, const T& value);
    template<typename ForwardIt>
    typename Vector<T, Alloc>::iterator reallocateAndInsertRange(std::size_t insertionIndex, std::size_t count, ForwardIt first, ForwardIt last);

    [[nodiscard]] bool isValidIterator(Vector<T, Alloc>::const_iterator pos) const;
    [[nodiscard]] std::size_t maxSize() const noexcept;
    [[nodiscard]] std::size_t nextCapacity(std::size_t required) const ;
    void checkGrowth(size_type count) const;
private:
    BufferStorage<T, Alloc> m_storage{};
    std::size_t m_size{};
};

template<typename T, typename Alloc>
void swap(Vector<T, Alloc>& lhs,
          Vector<T, Alloc>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}
#include "Vector.tpp"