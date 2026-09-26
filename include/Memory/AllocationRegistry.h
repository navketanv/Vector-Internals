#pragma once
#include <cstddef>
#include <unordered_map>
#include "AllocationStats.h"

class AllocationRegistry {
public:
    AllocationStats& statsFor(std::size_t id);
    const AllocationStats& statsFor(std::size_t id) const;
    AllocationStats* findStats(std::size_t id) noexcept;

    void reset(std::size_t id);
    void resetAll();

private:
    std::unordered_map<std::size_t, AllocationStats> m_stats;
};

inline AllocationStats& AllocationRegistry::statsFor(std::size_t id) {
    return m_stats[id];
}

inline const AllocationStats& AllocationRegistry::statsFor(std::size_t id) const {
    return m_stats.at(id);
}

inline void AllocationRegistry::reset(std::size_t id) {
    m_stats.erase(id);
}

inline void AllocationRegistry::resetAll() {
    m_stats.clear();
}

inline AllocationStats* AllocationRegistry::findStats(std::size_t id) noexcept {
    std::unordered_map<std::size_t, AllocationStats>::iterator it = m_stats.find(id);
    if (it != m_stats.end()) {
        return &it->second;
    }
    return nullptr;
}