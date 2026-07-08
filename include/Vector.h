#pragma once
#include "Allocator.h"
#include "AllocatorPolicy.h"
#include "BufferStorage.h"
#include "IteratorBase.h"
#include <initializer_list>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <iterator>

template<typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    using value_type = T;
    using allocator_type = Alloc;
    using AllocatorTraits = std::allocator_traits<allocator_type>;
    using AllocPolicy = AllocatorPolicy<allocator_type>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference = const T&;

    using pointer = T*;
    using const_pointer = const T*;

    using iterator = IteratorBase<T, false>;
    using const_iterator = IteratorBase<T, true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
static constexpr bool kVectorSwapNoexcept = std::is_nothrow_swappable_v<BufferStorage<T, Alloc>> &&
                                            std::is_nothrow_swappable_v<size_type>;

    template<std::forward_iterator ForwardIt>
    void assignFromRange(Vector<T, Alloc>::allocator_type& alloc, ForwardIt first, ForwardIt last);
    template<typename... Args>
    Vector<T, Alloc>::pointer constructOne(Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>::pointer dest, Args&&... args);
    // Default-constructs count objects beginning at dest.
    Vector<T, Alloc>::pointer constructDefault(Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>::pointer dest, Vector<T, Alloc>::size_type count);
    // Copy-constructs count copies of value beginning at dest.
    Vector<T, Alloc>::pointer constructFill(Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>::pointer dest, Vector<T, Alloc>::size_type count, const T& value);
    // Copy-constructs elements from the source range [first, last)
    // into uninitialized storage beginning at dest.
    // Used when constructing elements from an external source range.
    // Returns one-past-the-last successfully constructed element.
    // Destroys any partially constructed elements and rethrows on failure.
    template<std::input_iterator Iterator>
    Vector<T, Alloc>::pointer constructCopyRange(Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>::pointer dest, Iterator first, Iterator last);
    // Move-constructs elements from the source range [first, last)
    // into uninitialized storage beginning at dest.
    // Used when relocating existing elements during reallocation.
    // Returns one-past-the-last successfully constructed element.
    // Destroys any partially constructed elements and rethrows on failure.
    template<std::input_iterator Iterator>
    Vector<T, Alloc>::pointer constructMoveRange(Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>::pointer dest, Iterator first, Iterator last);
    void destroyRange(Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>::pointer first, Vector<T, Alloc>::pointer last) noexcept;

    // Private allocator-aware-constructors
    Vector(const Vector<T, Alloc>::allocator_type& alloc, const Vector<T, Alloc>& rhs);
    Vector(const Vector<T, Alloc>::allocator_type& alloc, Vector<T, Alloc>&& rhs);

public:
    Vector();
    Vector(Vector<T, Alloc>::size_type size, const T& value);
    explicit Vector(Vector<T, Alloc>::size_type size);
    Vector(std::initializer_list<T> list);
    template<std::input_iterator Iterator>
    Vector(Iterator first, Iterator last);
    Vector(const Vector<T, Alloc>& rhs);
    Vector(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<BufferStorage<T, Alloc>>);
    Vector<T, Alloc>& operator=(const Vector<T, Alloc>& rhs);
    Vector<T, Alloc>& operator=(Vector<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<BufferStorage<T, Alloc>>);
    ~Vector() noexcept;

    void reserve(Vector<T, Alloc>::size_type newCapacity);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, const T& value);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, T&& value);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, Vector<T, Alloc>::size_type count, const T& value);
    template<std::input_iterator Iterator>
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, Iterator first, Iterator last);
    typename Vector<T, Alloc>::iterator insert(Vector<T, Alloc>::const_iterator pos, std::initializer_list<T> ilist);
    typename Vector<T, Alloc>::iterator erase(Vector<T, Alloc>::const_iterator first, Vector<T, Alloc>::const_iterator last);
    typename Vector<T, Alloc>::iterator erase(Vector<T, Alloc>::const_iterator pos);

    void push_back(const T& value);
    void push_back(T&& value);

    template<typename... Args>
    typename Vector<T, Alloc>::reference emplace_back(Args&&... args);
    void pop_back();
    void resize(Vector<T, Alloc>::size_type count, const T& value);
    void resize(Vector<T, Alloc>::size_type count);
    void shrink_to_fit();
    void swap(Vector<T, Alloc>& rhs) noexcept(kVectorSwapNoexcept);
    void clear() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::size_type size() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::size_type capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] typename Vector<T, Alloc>::pointer data() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_pointer data() const noexcept;

    [[nodiscard]] typename Vector<T, Alloc>::allocator_type get_allocator() noexcept;
    [[nodiscard]] const typename Vector<T, Alloc>::allocator_type get_allocator() const noexcept;

    [[nodiscard]] typename Vector<T, Alloc>::reference operator[](Vector<T, Alloc>::size_type index) noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reference operator[](Vector<T, Alloc>::size_type index) const noexcept;

    [[nodiscard]] typename Vector<T, Alloc>::reference at(Vector<T, Alloc>::size_type index);
    [[nodiscard]] typename Vector<T, Alloc>::const_reference at(Vector<T, Alloc>::size_type index) const;

    [[nodiscard]] typename Vector<T, Alloc>::reference front() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reference front() const noexcept;

    [[nodiscard]] typename Vector<T, Alloc>::reference back() noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::const_reference back() const noexcept;

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

    void reallocateStorage(Vector<T, Alloc>::size_type newCapacity);
    template<typename... Args>
    typename Vector<T, Alloc>::iterator insertInPlace(Vector<T, Alloc>::size_type insertionIndex, Args&&... args);
    typename Vector<T, Alloc>::iterator insertInPlace(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, const T& value);
    template<typename ForwardIt>
    typename Vector<T, Alloc>::iterator insertRangeInPlace(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, ForwardIt first, ForwardIt last);
    template<typename... Args>
    typename Vector<T, Alloc>::iterator reallocateAndInsert(Vector<T, Alloc>::size_type insertionIndex, Args&&... args);
    typename Vector<T, Alloc>::iterator reallocateAndInsert(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, const T& value);
    template<typename ForwardIt>
    typename Vector<T, Alloc>::iterator reallocateAndInsertRange(Vector<T, Alloc>::size_type insertionIndex, Vector<T, Alloc>::size_type count, ForwardIt first, ForwardIt last);

    [[nodiscard]] bool isValidIterator(Vector<T, Alloc>::const_iterator pos) const;
    [[nodiscard]] typename Vector<T, Alloc>::allocator_type& allocator() noexcept;
    [[nodiscard]] const typename Vector<T, Alloc>::allocator_type& allocator() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::size_type max_size() const noexcept;
    [[nodiscard]] typename Vector<T, Alloc>::size_type nextCapacity(Vector<T, Alloc>::size_type required) const ;
    void checkGrowth(Vector<T, Alloc>::size_type count) const;
private:
    BufferStorage<T, Alloc> m_storage{};
    size_type m_size{};
};

template<typename T, typename Alloc>
void swap(Vector<T, Alloc>& lhs,
          Vector<T, Alloc>& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}
#include "Vector.tpp"