#include "Allocator.h"
#include <utility>

template<typename T>
T* Allocator<T>::allocate(std::size_t count) {
    return static_cast<T*>(::operator new(sizeof(T) * count));
}

template<typename T>
void Allocator<T>::deallocate(T* ptr, std::size_t count) noexcept {
    ::operator delete(ptr, (sizeof(T) * count));
}

template<typename T>
template<typename... Args>
void Allocator<T>::construct(T* ptr, Args&&... args) {
    if (ptr != nullptr) {
        ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}

template<typename T>
void Allocator<T>::destroy(T* ptr) noexcept {
    if (ptr != nullptr) {
        ptr->~T();
    }
}
