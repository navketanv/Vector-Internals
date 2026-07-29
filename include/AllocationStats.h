#pragma once
#include <cstddef>

struct AllocationStats {
    std::size_t allocations{};
    std::size_t deallocations{};
    std::size_t constructions{};
    std::size_t destructions{};
};
