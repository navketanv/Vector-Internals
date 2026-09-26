#pragma once
#include <cstddef>
#include <iterator>
#include <type_traits>

template<typename T, bool IsConst>
class IteratorBase {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;

    IteratorBase() = default;
    explicit IteratorBase(pointer ptr) noexcept : m_ptr(ptr) {}
    template<bool B = IsConst>
    IteratorBase(const IteratorBase<T, false>& rhs) noexcept requires(B) : m_ptr(rhs.base()) {}

    reference operator*() const noexcept { return *m_ptr; }
    pointer operator->() const noexcept { return m_ptr; }

    IteratorBase<T, IsConst>& operator++() noexcept {
        ++m_ptr;
        return *this;
    }
    IteratorBase<T, IsConst> operator++(int) noexcept {
        IteratorBase<T, IsConst> temp = *this;
        ++(*this);
        return temp;
    }
    IteratorBase<T, IsConst>& operator--() noexcept {
        --m_ptr;
        return *this;
    }
    IteratorBase<T, IsConst> operator--(int) noexcept {
        IteratorBase<T, IsConst> temp = *this;
        --(*this);
        return temp;
    }
    IteratorBase<T, IsConst>& operator+=(difference_type n) noexcept {
        m_ptr += n;
        return *this;
    }
    IteratorBase<T, IsConst>& operator-=(difference_type n) noexcept {
        m_ptr -= n;
        return *this;
    }
    IteratorBase<T, IsConst> operator+(difference_type n) const noexcept {
        IteratorBase<T, IsConst> temp = *this;
        temp += n;
        return temp;
    }
    IteratorBase<T, IsConst> operator-(difference_type n) const noexcept {
        IteratorBase<T, IsConst> temp = *this;
        temp -= n;
        return temp;
    }
    reference operator[](difference_type n) const noexcept {
        return m_ptr[n];
    }
    template<bool OtherConst>
    bool operator==(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr == rhs.base());
    }
    template<bool OtherConst>
    bool operator!=(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr != rhs.base());
    }
    template<bool OtherConst>
    bool operator<(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr < rhs.base());
    }
    template<bool OtherConst>
    bool operator<=(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr <= rhs.base());
    }
    template<bool OtherConst>
    bool operator>(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr > rhs.base());
    }
    template<bool OtherConst>
    bool operator>=(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr >= rhs.base());
    }
    template<bool OtherConst>
    difference_type operator-(const IteratorBase<T, OtherConst>& rhs) const noexcept {
        return (m_ptr - rhs.base());
    }
    [[nodiscard]] pointer base() const noexcept {
        return m_ptr;
    }
    friend IteratorBase<T, IsConst> operator+(difference_type n, const IteratorBase<T, IsConst>& rhs) noexcept {
        return (rhs + n);
    }
private:
    pointer m_ptr{};
};