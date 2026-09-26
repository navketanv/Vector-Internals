#pragma once
#include <cstddef>
#include <memory>
#include <type_traits>

#include "Memory/StatefulAllocator.h"
#include "Memory/TrackingAllocator.h"


template<typename Alloc>
class AllocatorFactory
{
public:
    [[nodiscard]]
    static Alloc create() noexcept(std::is_nothrow_default_constructible_v<Alloc>)
    {
        return Alloc{};
    }
};

template<typename T>
class AllocatorFactory<StatefulAllocator<T>>
{
public:
    [[nodiscard]]
    static StatefulAllocator<T> create(std::size_t id = 42) noexcept
    {
        return StatefulAllocator<T>{id};
    }
};

template<typename T>
class AllocatorFactory<TrackingAllocator<T>>
{
public:
    [[nodiscard]]
    static TrackingAllocator<T> create(std::size_t id = 42, std::shared_ptr<AllocationRegistry> pRegistry = nullptr) noexcept
    {
        return TrackingAllocator<T>{id, pRegistry};
    }
};
