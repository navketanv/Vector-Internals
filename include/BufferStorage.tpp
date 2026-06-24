#include <utility>

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(BufferStorage<T, Alloc>::size_type capacity)
    : m_allocator{}
//    , m_pData(capacity ? m_allocator.allocate(capacity) : nullptr)
    , m_pData(capacity ? BufferStorage<T, Alloc>::AllocatorTraits::allocate(m_allocator, capacity) : nullptr)
    , m_capacity(capacity) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(const BufferStorage<T, Alloc>::allocator_type& alloc, BufferStorage<T, Alloc>::size_type capacity)
    : m_allocator(alloc)
    , m_pData(capacity ? BufferStorage<T, Alloc>::AllocatorTraits::allocate(m_allocator, capacity) : nullptr)
    , m_capacity(capacity) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::BufferStorage(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Alloc>)
    : m_allocator(std::move(rhs.m_allocator))
    , m_pData(std::exchange(rhs.m_pData, nullptr))
    , m_capacity(std::exchange(rhs.m_capacity, 0)) {}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>& BufferStorage<T, Alloc>::operator=(BufferStorage<T, Alloc>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Alloc>) {
    if (this != &rhs) {
        releaseStorage();
        m_allocator = std::move(rhs.m_allocator);
        m_pData = std::exchange(rhs.m_pData, nullptr);
        m_capacity = std::exchange(rhs.m_capacity, 0);
    }
    return *this;
}

template<typename T, typename Alloc>
BufferStorage<T, Alloc>::~BufferStorage<T, Alloc>() {
    releaseStorage();
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::swap(BufferStorage<T, Alloc>& rhs) noexcept(kStorageSwapNoexcept) {
    using std::swap;
    // allocator and storage must remain paired
    swap(m_allocator, rhs.m_allocator);
    swap(m_pData, rhs.m_pData);
    swap(m_capacity, rhs.m_capacity);
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::stealStorage(BufferStorage<T, Alloc>& rhs) noexcept {
    releaseStorage();
    m_pData = std::exchange(rhs.m_pData, nullptr);
    m_capacity = std::exchange(rhs.m_capacity, 0);
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::pointer
BufferStorage<T, Alloc>::data() noexcept {
    return m_pData;
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::const_pointer
BufferStorage<T, Alloc>::data() const noexcept {
    return m_pData;
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::allocator_type&
BufferStorage<T, Alloc>::allocator() noexcept {
    return m_allocator;
}

template<typename T, typename Alloc>
const typename BufferStorage<T, Alloc>::allocator_type&
BufferStorage<T, Alloc>::allocator() const noexcept {
    return m_allocator;
}

template<typename T, typename Alloc>
typename BufferStorage<T, Alloc>::size_type
BufferStorage<T, Alloc>::capacity() const noexcept {
    return m_capacity;
}

template<typename T, typename Alloc>
bool BufferStorage<T, Alloc>::hasStorage() const noexcept {
    return m_pData != nullptr;
}

template<typename T, typename Alloc>
void BufferStorage<T, Alloc>::releaseStorage() noexcept {
    if (m_pData != nullptr) {
        BufferStorage<T, Alloc>::AllocatorTraits::deallocate(m_allocator, m_pData, m_capacity);
        m_pData = nullptr;
        m_capacity = 0;
        //m_allocator.deallocate(m_pData, m_capacity);
    }
}