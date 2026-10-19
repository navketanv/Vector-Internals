#pragma once
#include <cstddef>
#include <limits>
#include <stdexcept>

namespace Memory {

struct PoolLayout {
    static constexpr std::size_t defaultChunkSizeBytes{4096};
    std::size_t chunkSizeBytes{defaultChunkSizeBytes};
    std::size_t slotSizeBytes{};
    std::size_t slotAlignment{};
    static constexpr std::size_t maxValue = std::numeric_limits<std::size_t>::max();

    [[nodiscard]] constexpr std::size_t effectiveSlotSize() const noexcept {
        return slotSizeBytes < sizeof(void*) ? sizeof(void*) : slotSizeBytes;
    }

    [[nodiscard]] constexpr std::size_t effectiveSlotAlignment() const noexcept {
        return slotAlignment < alignof(void*) ? alignof(void*) : slotAlignment;
    }


    [[nodiscard]] constexpr bool isValid() const noexcept {
        if ((chunkSizeBytes == 0) || (slotSizeBytes == 0) || (slotAlignment == 0)) {
            return false;
        }

        if ((slotAlignment & (slotAlignment - 1)) != 0) {
            return false;
        }

        const std::size_t alignment{effectiveSlotAlignment()};
        const std::size_t size{effectiveSlotSize()};

        if ((alignment & (alignment - 1)) != 0) {
            return false;
        }

        if (size > (maxValue - (alignment - 1))) {
            return false;
        }

        const std::size_t stride{
            (size + (alignment - 1)) & ~(alignment - 1)
        };

        return stride <= chunkSizeBytes;
    }

    [[nodiscard]] constexpr std::size_t slotStrideBytes() const noexcept {
        if (!isValid()) {
            return 0;
        }

        const std::size_t alignment{effectiveSlotAlignment()};
        const std::size_t size{effectiveSlotSize()};

        return (size + (alignment - 1)) & ~(alignment - 1);
    }

    [[nodiscard]] constexpr std::size_t slotsPerChunk() const noexcept {
        const std::size_t stride{slotStrideBytes()};

        if (stride == 0) {
            return 0;
        }

        return chunkSizeBytes / stride;
    }

    void validate() const {
        if (!isValid()) {
            throw std::invalid_argument("PoolLayout: invalid chunk size, slot size or alignment");
        }
    }
};
} // namespace Memory