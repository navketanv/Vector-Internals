#include "Allocator.h"
#include <new>
#include <limits>
#include <utility>

template<typename T>
T* Allocator<T>::allocate(std::size_t count) {
    if (count == 0) {
        return nullptr;
    }

    if (count > maxSize()) {
        throw std::bad_array_new_length();
    }
    return static_cast<T*>(::operator new(sizeof(T) * count));
}

template<typename T>
void Allocator<T>::deallocate(T* ptr, std::size_t count) noexcept {
    ::operator delete(ptr, (sizeof(T) * count));
}
/*
template<typename T>
template<typename... Args>
void Allocator<T>::construct(T* ptr, Args&&... args) {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}
*/
template<typename T>
void Allocator<T>::destroy(T* ptr) noexcept {
    if (ptr != nullptr) {
        ptr->~T();
    }
}

template<typename T>
std::size_t Allocator<T>::maxSize() const noexcept {
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}

template class Allocator<int>;
template class Allocator<double>;
