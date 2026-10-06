#include <gtest/gtest.h>
#include <utility>
#include <memory>

#include "Containers/Vector.h"
#include "Memory/Allocator.h"
#include "Memory/StatefulAllocator.h"
#include "Memory/TrackingAllocator.h"
#include "Memory/AllocationRegistry.h"
#include "Memory/AllocationStats.h"
#include "Memory/BufferStorage.h"

struct LifeTimeTracker
{
    static inline std::size_t constructions{};
    static inline std::size_t destructions{};
    LifeTimeTracker() { ++constructions; }
    ~LifeTimeTracker() { ++destructions; }
};

// VectorTests.cpp
using TrackingIntAllocator = TrackingAllocator<int>;
using BufferStorageIntTracking = BufferStorage<int, TrackingAllocator<int>>;
using TrackingIntVector = Vector<int, TrackingAllocator<int>>;

using StatefulIntAllocator = StatefulAllocator<int>;
using BufferStorageIntStateful = BufferStorage<int, StatefulAllocator<int>>;
using StatefulIntVector = Vector<int, StatefulAllocator<int>>;

TEST(VectorTests, DefaultAllocatorDefaultVectorConstruction)
{
    Vector<int> vec;
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_EQ(vec.data(), nullptr);

    Allocator<int> allocator{};
    Vector<int> vec2(allocator);
    EXPECT_EQ(vec2.size(), 0);
    EXPECT_EQ(vec2.capacity(), 0);
    EXPECT_EQ(vec2.data(), nullptr);
}

TEST(VectorTests, StatefulIntAllocatorDefaultVectorConstruction)
{
    StatefulIntVector vec;
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_EQ(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);

    StatefulIntAllocator allocator(13);
    StatefulIntVector vec2(allocator);
    EXPECT_EQ(vec2.size(), 0);
    EXPECT_EQ(vec2.capacity(), 0);
    EXPECT_EQ(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 13);
}

TEST(VectorTests, TrackingIntAllocatorDefaultVectorConstruction)
{
    TrackingIntVector vec;
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_EQ(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);

    TrackingIntAllocator allocator(13, nullptr);
    TrackingIntVector vec2(allocator);
    EXPECT_EQ(vec2.size(), 0);
    EXPECT_EQ(vec2.capacity(), 0);
    EXPECT_EQ(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 13);

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    EXPECT_NE(pRegistry, nullptr);
    TrackingIntAllocator allocator2(33, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator2.id());
    {
        TrackingIntVector vec3(allocator2);
        EXPECT_EQ(vec3.size(), 0);
        EXPECT_EQ(vec3.capacity(), 0);
        EXPECT_EQ(vec3.data(), nullptr);
        EXPECT_EQ(vec3.get_allocator().id(), 33);

        EXPECT_EQ(stats.allocations, 0);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 0);
        EXPECT_EQ(stats.destructions, 0);
    }
    EXPECT_EQ(stats.allocations, 0);
    EXPECT_EQ(stats.deallocations, 0);
    EXPECT_EQ(stats.constructions, 0);
    EXPECT_EQ(stats.destructions, 0);
}

TEST(VectorTests, DefaultAllocatorSizeAndCapacityConstruction)
{
    Vector<int> vec(10);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    EXPECT_NE(vec.data(), nullptr);

    Allocator<int> allocator{};
    Vector<int> vec2(23, allocator);
    EXPECT_EQ(vec2.size(), 23);
    EXPECT_EQ(vec2.capacity(), 23);
    EXPECT_NE(vec2.data(), nullptr);
}

TEST(VectorTests, StatefulIntAllocatorSizeAndCapacityVectorConstruction)
{
    StatefulIntVector vec(10);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    EXPECT_NE(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);

    StatefulIntAllocator allocator(19);
    StatefulIntVector vec2(17, allocator);
    EXPECT_EQ(vec2.size(), 17);
    EXPECT_EQ(vec2.capacity(), 17);
    EXPECT_NE(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 19);
}

TEST(VectorTests, TrackingIntAllocatorSizeAndCapacityVectorConstruction)
{
    TrackingIntVector vec(10);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    EXPECT_NE(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);

    TrackingIntAllocator allocator(29, nullptr);
    TrackingIntVector vec2(31, allocator);
    EXPECT_EQ(vec2.size(), 31);
    EXPECT_EQ(vec2.capacity(), 31);
    EXPECT_NE(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 29);

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    EXPECT_NE(pRegistry, nullptr);
    TrackingIntAllocator allocator2(47, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator2.id());
    {
        TrackingIntVector vec3(39, allocator2);
        EXPECT_EQ(vec3.size(), 39);
        EXPECT_EQ(vec3.capacity(), 39);
        EXPECT_NE(vec3.data(), nullptr);
        EXPECT_EQ(vec3.get_allocator().id(), 47);

        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 39);
        EXPECT_EQ(stats.destructions, 0);
    }
    EXPECT_EQ(stats.allocations, 1);
    EXPECT_EQ(stats.deallocations, 1);
    EXPECT_EQ(stats.constructions, 39);
    EXPECT_EQ(stats.destructions, 39);
}

TEST(VectorTests, DefaultAllocatorSizeAndCapacityWithValueConstruction)
{
    Vector<int> vec(10, 91);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    EXPECT_NE(vec.data(), nullptr);
    for (const auto& item : vec) {
        EXPECT_EQ(item, 91);
    }

    Allocator<int> allocator{};
    Vector<int> vec2(21, 73, allocator);
    EXPECT_EQ(vec2.size(), 21);
    EXPECT_EQ(vec2.capacity(), 21);
    EXPECT_NE(vec2.data(), nullptr);
    for (const auto& item : vec2) {
        EXPECT_EQ(item, 73);
    }
}

TEST(VectorTests, StatefulIntAllocatorSizeAndCapacityWithValueConstruction)
{
    StatefulIntVector vec(10, 91);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    EXPECT_NE(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);
    for (const auto& item : vec) {
        EXPECT_EQ(item, 91);
    }

    StatefulIntAllocator allocator{19};
    StatefulIntVector vec2(23, 79, allocator);
    EXPECT_EQ(vec2.size(), 23);
    EXPECT_EQ(vec2.capacity(), 23);
    EXPECT_NE(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 19);
    for (const auto& item : vec2) {
        EXPECT_EQ(item, 79);
    }
}

TEST(VectorTests, TrackingIntAllocatorSizeAndCapacityWithValueConstruction)
{
    TrackingIntVector vec(29, 91);
    EXPECT_EQ(vec.size(), 29);
    EXPECT_EQ(vec.capacity(), 29);
    EXPECT_NE(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);
    for (const auto& item : vec) {
        EXPECT_EQ(item, 91);
    }

    TrackingIntAllocator allocator(77, nullptr);
    TrackingIntVector vec2(41, 103, allocator);
    EXPECT_EQ(vec2.size(), 41);
    EXPECT_EQ(vec2.capacity(), 41);
    EXPECT_NE(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 77);
    for (const auto& item : vec2) {
        EXPECT_EQ(item, 103);
    }

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    TrackingIntAllocator allocator2(31, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator2.id());

    {
        TrackingIntVector vec3(109, 133, allocator2);
        EXPECT_EQ(vec3.size(), 109);
        EXPECT_EQ(vec3.capacity(), 109);
        EXPECT_NE(vec3.data(), nullptr);
        EXPECT_EQ(vec3.get_allocator().id(), 31);
        for (const auto& item : vec3) {
            EXPECT_EQ(item, 133);
        }
        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 109);
        EXPECT_EQ(stats.destructions, 0);
    }
    EXPECT_EQ(stats.allocations, 1);
    EXPECT_EQ(stats.deallocations, 1);
    EXPECT_EQ(stats.constructions, 109);
    EXPECT_EQ(stats.destructions, 109);
}

TEST(VectorTests, DefaultAllocatorSizeAndCapacityWithInitializerListConstruction)
{
    Vector<int> vec{10, 20, 30, 40, 50};
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec.capacity(), 5);
    EXPECT_NE(vec.data(), nullptr);
    for (std::size_t index = 0; index < vec.size(); ++index) {
        EXPECT_EQ(vec[index], (index + 1) * 10);
    }

    Allocator<int> allocator{};
    Vector<int> vec2({10, 21, 32, 43, 54, 65}, allocator);
    EXPECT_EQ(vec2.size(), 6);
    EXPECT_EQ(vec2.capacity(), 6);
    EXPECT_NE(vec2.data(), nullptr);
    for (std::size_t index = 0; index < vec2.size(); ++index) {
        EXPECT_EQ(vec2[index], (index + 1) * 10 + index);
    }
}

TEST(VectorTests, StatefulIntAllocatorSizeAndCapacityWithInitializerListConstruction)
{
    StatefulIntVector vec{10, 20, 30, 40, 50};
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec.capacity(), 5);
    EXPECT_NE(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);
    for (std::size_t index = 0; index < vec.size(); ++index) {
        EXPECT_EQ(vec[index], (index + 1) * 10);
    }

    StatefulIntAllocator allocator(23);
    StatefulIntVector vec2({1, 3, 7, 13, 21, 31}, allocator);
    EXPECT_EQ(vec2.size(), 6);
    EXPECT_EQ(vec2.capacity(), 6);
    EXPECT_NE(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 23);
    for (std::size_t index = 0; index < vec2.size(); ++index) {
        EXPECT_EQ(vec2[index], (index * index + index + 1));
    }

    StatefulIntAllocator allocator2(33);
    StatefulIntVector vec3({10, 21, 32, 43, 54, 65}, allocator2);
    EXPECT_EQ(vec3.size(), 6);
    EXPECT_EQ(vec3.capacity(), 6);
    EXPECT_NE(vec3.data(), nullptr);
    EXPECT_EQ(vec3.get_allocator().id(), 33);
    for (std::size_t index = 0; index < vec3.size(); ++index) {
        EXPECT_EQ(vec3[index], (index + 1) * 10 + index);
    }
}

TEST(VectorTests, TrackingIntAllocatorSizeAndCapacityWithInitializerListConstruction)
{
    TrackingIntVector vec{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    EXPECT_NE(vec.data(), nullptr);
    EXPECT_EQ(vec.get_allocator().id(), 42);
    for (std::size_t index = 0; index < vec.size(); ++index) {
        EXPECT_EQ(vec[index], (index + 1) * 10);
    }

    TrackingIntAllocator allocator(41, nullptr);
    TrackingIntVector vec2({1, 3, 7, 13, 21, 31}, allocator);
    EXPECT_EQ(vec2.size(), 6);
    EXPECT_EQ(vec2.capacity(), 6);
    EXPECT_NE(vec2.data(), nullptr);
    EXPECT_EQ(vec2.get_allocator().id(), 41);
    for (std::size_t index = 0; index < vec2.size(); ++index) {
        EXPECT_EQ(vec2[index], (index * index + index + 1));
    }

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    EXPECT_NE(pRegistry, nullptr);
    TrackingIntAllocator allocator2(133, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator2.id());
    {
        TrackingIntVector vec3({10, 21, 32, 43, 54, 65}, allocator2);
        EXPECT_EQ(vec3.size(), 6);
        EXPECT_EQ(vec3.capacity(), 6);
        EXPECT_NE(vec3.data(), nullptr);
        EXPECT_EQ(vec3.get_allocator().id(), 133);
        for (std::size_t index = 0; index < vec3.size(); ++index) {
            EXPECT_EQ(vec3[index], (index + 1) * 10 + index);
        }
        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 6);
        EXPECT_EQ(stats.destructions, 0);
    }
    EXPECT_EQ(stats.allocations, 1);
    EXPECT_EQ(stats.deallocations, 1);
    EXPECT_EQ(stats.constructions, 6);
    EXPECT_EQ(stats.destructions, 6);
}

TEST(VectorTests, SizeConstructionCreatesAndDestroysObjects)
{
    LifeTimeTracker::constructions = 0;
    LifeTimeTracker::destructions = 0;

    {
        Vector<LifeTimeTracker> vec(10);
        EXPECT_EQ(vec.size(), 10);
        EXPECT_EQ(vec.capacity(), 10);
        EXPECT_NE(vec.data(), nullptr);
        EXPECT_EQ(LifeTimeTracker::constructions, 10);
        EXPECT_EQ(LifeTimeTracker::destructions, 0);
    }
    EXPECT_EQ(LifeTimeTracker::constructions, 10);
    EXPECT_EQ(LifeTimeTracker::destructions, 10);
}

TEST(VectorTests, StatefulIntAllocatorCopyConstruction)
{
    StatefulIntVector source{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(source.size(), 10);
    EXPECT_EQ(source.capacity(), 10);
    EXPECT_NE(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(source[index], (index + 1) * 10);
    }

    StatefulIntVector copy(source);
    EXPECT_EQ(copy.size(), source.size());
    EXPECT_EQ(copy.capacity(), source.capacity());
    EXPECT_NE(copy.data(), nullptr);
    EXPECT_NE(copy.data(), source.data());
    EXPECT_EQ(copy.get_allocator().id(), source.get_allocator().id());
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(copy[index], source[index]);
    }

    StatefulIntAllocator allocator(73);
    StatefulIntVector source2({10, 20, 30, 40, 50, 60}, allocator);
    EXPECT_EQ(source2.size(), 6);
    EXPECT_EQ(source2.capacity(), 6);
    EXPECT_NE(source2.data(), nullptr);
    EXPECT_EQ(source2.get_allocator().id(), 73);
    for (std::size_t index = 0; index < source2.size(); ++index) {
        EXPECT_EQ(source2[index], (index + 1) * 10);
    }

    StatefulIntVector copy2(source2);
    EXPECT_EQ(copy2.size(), source2.size());
    EXPECT_EQ(copy2.capacity(), source2.capacity());
    EXPECT_NE(copy2.data(), nullptr);
    EXPECT_NE(copy2.data(), source2.data());
    EXPECT_EQ(copy2.get_allocator().id(), source2.get_allocator().id());
    for (std::size_t index = 0; index < source2.size(); ++index) {
        EXPECT_EQ(source2[index], copy2[index]);
    }

    StatefulIntAllocator allocator2(19);
    StatefulIntVector copy3(source2, allocator2);
    EXPECT_EQ(copy3.size(), source2.size());
    EXPECT_EQ(copy3.capacity(), source2.capacity());
    EXPECT_NE(copy3.data(), nullptr);
    EXPECT_NE(copy3.data(), source2.data());
    EXPECT_EQ(copy3.get_allocator().id(), 19);
    for (std::size_t index = 0; index < source2.size(); ++index) {
        EXPECT_EQ(copy3[index], source2[index]);
    }
}

TEST(VectorTests, TrackingIntAllocatorCopyConstruction)
{
    TrackingIntVector source{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(source.size(), 10);
    EXPECT_EQ(source.capacity(), 10);
    EXPECT_NE(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(source[index], (index + 1) * 10);
    }

    TrackingIntVector copy(source);
    EXPECT_EQ(copy.size(), source.size());
    EXPECT_EQ(copy.capacity(), source.capacity());
    EXPECT_NE(copy.data(), nullptr);
    EXPECT_NE(copy.data(), source.data());
    EXPECT_EQ(copy.get_allocator().id(), source.get_allocator().id());
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(copy[index], source[index]);
    }

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    EXPECT_NE(pRegistry, nullptr);
    TrackingIntAllocator allocator(173, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator.id());
    {
        TrackingIntVector source2({10, 20, 30, 40, 50, 60}, allocator);
        EXPECT_EQ(source2.size(), 6);
        EXPECT_EQ(source2.capacity(), 6);
        EXPECT_NE(source2.data(), nullptr);
        EXPECT_EQ(source2.get_allocator().id(), 173);
        for (std::size_t index = 0; index < source2.size(); ++index) {
            EXPECT_EQ(source2[index], (index + 1) * 10);
        }
        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 6);
        EXPECT_EQ(stats.destructions, 0);

        {
            TrackingIntVector copy2(source2);
            EXPECT_EQ(copy2.size(), source2.size());
            EXPECT_EQ(copy2.capacity(), source2.capacity());
            EXPECT_NE(copy2.data(), nullptr);
            EXPECT_NE(copy2.data(), source2.data());
            EXPECT_EQ(copy2.get_allocator().id(), source2.get_allocator().id());
            for (std::size_t index = 0; index < source2.size(); ++index) {
                EXPECT_EQ(source2[index], copy2[index]);
            }
            EXPECT_EQ(stats.allocations, 2);
            EXPECT_EQ(stats.deallocations, 0);
            EXPECT_EQ(stats.constructions, 12);
            EXPECT_EQ(stats.destructions, 0);
        }
        EXPECT_EQ(stats.allocations, 2);
        EXPECT_EQ(stats.deallocations, 1);
        EXPECT_EQ(stats.constructions, 12);
        EXPECT_EQ(stats.destructions, 6);

        EXPECT_NE(pRegistry, nullptr);
        TrackingIntAllocator allocator2(19, pRegistry);
        const AllocationStats& stats2 = pRegistry->statsFor(allocator2.id());
        {
            TrackingIntVector copy3(source2, allocator2);
            EXPECT_EQ(copy3.size(), source2.size());
            EXPECT_EQ(copy3.capacity(), source2.capacity());
            EXPECT_NE(copy3.data(), nullptr);
            EXPECT_EQ(source2.get_allocator().id(), 173);
            EXPECT_EQ(copy3.get_allocator().id(), 19);
            for (std::size_t index = 0; index < source2.size(); ++index) {
                EXPECT_EQ(copy3[index], source2[index]);
            }
            EXPECT_EQ(stats.allocations, 2);
            EXPECT_EQ(stats.deallocations, 1);
            EXPECT_EQ(stats.constructions, 12);
            EXPECT_EQ(stats.destructions, 6);
            EXPECT_EQ(stats2.allocations, 1);
            EXPECT_EQ(stats2.deallocations, 0);
            EXPECT_EQ(stats2.constructions, 6);
            EXPECT_EQ(stats2.destructions, 0);
        }
        EXPECT_EQ(stats.allocations, 2);
        EXPECT_EQ(stats.deallocations, 1);
        EXPECT_EQ(stats.constructions, 12);
        EXPECT_EQ(stats.destructions, 6);
        EXPECT_EQ(stats2.allocations, 1);
        EXPECT_EQ(stats2.deallocations, 1);
        EXPECT_EQ(stats2.constructions, 6);
        EXPECT_EQ(stats2.destructions, 6);
    }
    EXPECT_EQ(stats.allocations, 2);
    EXPECT_EQ(stats.deallocations, 2);
    EXPECT_EQ(stats.constructions, 12);
    EXPECT_EQ(stats.destructions, 12);
}

TEST(VectorTests, StatefulIntAllocatorMoveConstruction)
{
    StatefulIntVector source{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(source.size(), 10);
    EXPECT_EQ(source.capacity(), 10);
    EXPECT_NE(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(source[index], (index + 1) * 10);
    }

    StatefulIntVector move(std::move(source));
    EXPECT_EQ(move.size(), 10);
    EXPECT_EQ(move.capacity(), 10);
    EXPECT_EQ(source.size(), 0);
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_NE(move.data(), nullptr);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(move.get_allocator().id(), 42);
    for (std::size_t index = 0; index < move.size(); ++index) {
        EXPECT_EQ(move[index], (index + 1) * 10);
    }

    StatefulIntAllocator allocator(73);
    StatefulIntVector source2({10, 21, 32, 43, 54, 65}, allocator);
    EXPECT_EQ(source2.size(), 6);
    EXPECT_EQ(source2.capacity(), 6);
    EXPECT_NE(source2.data(), nullptr);
    EXPECT_EQ(source2.get_allocator().id(), 73);
    for (std::size_t index = 0; index < source2.size(); ++index) {
        EXPECT_EQ(source2[index], ((index + 1) * 10 + index));
    }

    StatefulIntVector move2(std::move(source2));
    EXPECT_EQ(move2.size(), 6);
    EXPECT_EQ(move2.capacity(), 6);
    EXPECT_NE(move2.data(), nullptr);
    EXPECT_EQ(source2.size(), 0);
    EXPECT_EQ(source2.capacity(), 0);
    EXPECT_EQ(source2.data(), nullptr);
    EXPECT_EQ(move2.get_allocator().id(), 73);
    for (std::size_t index = 0; index < move2.size(); ++index) {
        EXPECT_EQ(move2[index], ((index + 1) * 10 + index));
    }

    StatefulIntAllocator allocator2(91);
    StatefulIntVector source3({10, 21, 32, 43, 54, 65}, allocator2);
    StatefulIntAllocator allocator3(19);
    StatefulIntVector move3(std::move(source3), allocator3);
    EXPECT_EQ(move3.size(), 6);
    EXPECT_EQ(move3.capacity(), 6);
    EXPECT_NE(move3.data(), nullptr);
    EXPECT_EQ(source3.size(), 0);
    EXPECT_EQ(source3.capacity(), 0);
    EXPECT_EQ(source3.data(), nullptr);
    EXPECT_EQ(source3.get_allocator().id(), 91);
    EXPECT_EQ(move3.get_allocator().id(), 19);
    for (std::size_t index = 0; index < move3.size(); ++index) {
        EXPECT_EQ(move3[index], ((index + 1) * 10 + index));
    }
}

TEST(VectorTests, TrackingIntAllocatorMoveConstruction)
{
    TrackingIntVector source{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(source.size(), 10);
    EXPECT_EQ(source.capacity(), 10);
    EXPECT_NE(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(source[index], (index + 1) * 10);
    }

    TrackingIntVector move(std::move(source));
    EXPECT_EQ(move.size(), 10);
    EXPECT_EQ(move.capacity(), 10);
    EXPECT_EQ(source.size(), 0);
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_NE(move.data(), nullptr);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(move.get_allocator().id(), 42);
    for (std::size_t index = 0; index < move.size(); ++index) {
        EXPECT_EQ(move[index], (index + 1) * 10);
    }

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    TrackingIntAllocator allocator(73, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator.id());
    {
        TrackingIntVector source2({10, 21, 32, 43, 54, 65}, allocator);
        EXPECT_EQ(source2.size(), 6);
        EXPECT_EQ(source2.capacity(), 6);
        EXPECT_NE(source2.data(), nullptr);
        EXPECT_EQ(source2.get_allocator().id(), 73);
        for (std::size_t index = 0; index < source2.size(); ++index) {
            EXPECT_EQ(source2[index], ((index + 1) * 10 + index));
        }
        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 6);
        EXPECT_EQ(stats.destructions, 0);

        TrackingIntVector move2(std::move(source2));
        EXPECT_EQ(move2.size(), 6);
        EXPECT_EQ(move2.capacity(), 6);
        EXPECT_NE(move2.data(), nullptr);
        EXPECT_EQ(source2.size(), 0);
        EXPECT_EQ(source2.capacity(), 0);
        EXPECT_EQ(source2.data(), nullptr);
        EXPECT_EQ(move2.get_allocator().id(), 73);
        for (std::size_t index = 0; index < move2.size(); ++index) {
            EXPECT_EQ(move2[index], ((index + 1) * 10 + index));
        }
        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 6);
        EXPECT_EQ(stats.destructions, 0);
    }
    EXPECT_EQ(stats.allocations, 1);
    EXPECT_EQ(stats.deallocations, 1);
    EXPECT_EQ(stats.constructions, 6);
    EXPECT_EQ(stats.destructions, 6);


    TrackingIntAllocator allocator2(91, pRegistry);
    const AllocationStats& stats2 = pRegistry->statsFor(allocator2.id());
    {
        TrackingIntVector source3({10, 21, 32, 43, 54, 65}, allocator2);
        EXPECT_EQ(source3.size(), 6);
        EXPECT_EQ(source3.capacity(), 6);
        EXPECT_NE(source3.data(), nullptr);
        EXPECT_EQ(source3.get_allocator().id(), 91);
        for (std::size_t index = 0; index < source3.size(); ++index) {
            EXPECT_EQ(source3[index], ((index + 1) * 10 + index));
        }
        EXPECT_EQ(stats2.allocations, 1);
        EXPECT_EQ(stats2.deallocations, 0);
        EXPECT_EQ(stats2.constructions, 6);
        EXPECT_EQ(stats2.destructions, 0);

        TrackingIntAllocator allocator3(19, pRegistry);
        const AllocationStats& stats3 = pRegistry->statsFor(allocator3.id());
        {
            TrackingIntVector move3(std::move(source3), allocator3);
            EXPECT_EQ(move3.size(), 6);
            EXPECT_EQ(move3.capacity(), 6);
            EXPECT_NE(move3.data(), nullptr);
            EXPECT_EQ(source3.size(), 0);
            EXPECT_EQ(source3.capacity(), 0);
            EXPECT_EQ(source3.data(), nullptr);
            EXPECT_EQ(source3.get_allocator().id(), 91);
            EXPECT_EQ(move3.get_allocator().id(), 19);
            for (std::size_t index = 0; index < move3.size(); ++index) {
                EXPECT_EQ(move3[index], ((index + 1) * 10 + index));
            }
            EXPECT_EQ(stats2.allocations, 1);
            EXPECT_EQ(stats2.deallocations, 1);
            EXPECT_EQ(stats2.constructions, 6);
            EXPECT_EQ(stats2.destructions, 6);
            EXPECT_EQ(stats3.allocations, 1);
            EXPECT_EQ(stats3.deallocations, 0);
            EXPECT_EQ(stats3.constructions, 6);
            EXPECT_EQ(stats3.destructions, 0);
        }
        EXPECT_EQ(stats2.allocations, 1);
        EXPECT_EQ(stats2.deallocations, 1);
        EXPECT_EQ(stats2.constructions, 6);
        EXPECT_EQ(stats2.destructions, 6);
        EXPECT_EQ(stats3.allocations, 1);
        EXPECT_EQ(stats3.deallocations, 1);
        EXPECT_EQ(stats3.constructions, 6);
        EXPECT_EQ(stats3.destructions, 6);
    }
}

TEST(VectorTests, StatefulIntAllocatorMoveAssignmentOperator)
{
    StatefulIntVector source{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(source.size(), 10);
    EXPECT_EQ(source.capacity(), 10);
    EXPECT_NE(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(source[index], (index + 1) * 10);
    }

    StatefulIntVector destination{1, 2, 3};
    EXPECT_EQ(destination.size(), 3);
    EXPECT_EQ(destination.capacity(), 3);
    EXPECT_NE(destination.data(), nullptr);
    EXPECT_EQ(destination.get_allocator().id(), 42);
    for (std::size_t index = 0; index < destination.size(); ++index) {
        EXPECT_EQ(destination[index], index + 1);
    }

    destination = std::move(source);
    EXPECT_EQ(destination.size(), 10);
    EXPECT_EQ(destination.capacity(), 10);
    EXPECT_NE(destination.data(), nullptr);
    EXPECT_EQ(destination.get_allocator().id(), 42);
    for (std::size_t index = 0; index < destination.size(); ++index) {
        EXPECT_EQ(destination[index], (index + 1) * 10);
    }

    EXPECT_EQ(source.size(), 0);
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);

    StatefulIntAllocator allocator(91);
    StatefulIntVector source2({10, 21, 32, 43, 54, 65}, allocator);
    EXPECT_EQ(source2.size(), 6);
    EXPECT_EQ(source2.capacity(), 6);
    EXPECT_NE(source2.data(), nullptr);
    EXPECT_EQ(source2.get_allocator().id(), 91);
    for (std::size_t index = 0; index < source2.size(); ++index) {
        EXPECT_EQ(source2[index], ((index + 1) * 10 + index));
    }

    StatefulIntVector destination2(20, 13, allocator);
    EXPECT_EQ(destination2.size(), 20);
    EXPECT_EQ(destination2.capacity(), 20);
    EXPECT_NE(destination2.data(), nullptr);
    EXPECT_EQ(destination2.get_allocator().id(), 91);
    for (std::size_t index = 0; index < destination2.size(); ++index) {
        EXPECT_EQ(destination2[index], 13);
    }

    destination2 = std::move(source2);
    EXPECT_EQ(destination2.size(), 6);
    EXPECT_EQ(destination2.capacity(), 6);
    EXPECT_NE(destination2.data(), nullptr);
    EXPECT_EQ(destination2.get_allocator().id(), 91);
    for (std::size_t index = 0; index < destination2.size(); ++index) {
        EXPECT_EQ(destination2[index], ((index + 1) * 10 + index));
    }

    EXPECT_EQ(source2.size(), 0);
    EXPECT_EQ(source2.capacity(), 0);
    EXPECT_EQ(source2.data(), nullptr);
    EXPECT_EQ(source2.get_allocator().id(), 91);

    StatefulIntAllocator srcAllocator(73);
    StatefulIntVector source3({10, 21, 32, 43, 54, 65}, srcAllocator);
    EXPECT_EQ(source3.size(), 6);
    EXPECT_EQ(source3.capacity(), 6);
    EXPECT_NE(source3.data(), nullptr);
    EXPECT_EQ(source3.get_allocator().id(), 73);
    for (std::size_t index = 0; index < source3.size(); ++index) {
        EXPECT_EQ(source3[index], ((index + 1) * 10 + index));
    }

    StatefulIntAllocator destAllocator(37);
    StatefulIntVector destination3({10, 20, 30, 40, 50, 60, 70, 80, 90, 100}, destAllocator);
    EXPECT_EQ(destination3.size(), 10);
    EXPECT_EQ(destination3.capacity(), 10);
    EXPECT_NE(destination3.data(), nullptr);
    EXPECT_EQ(destination3.get_allocator().id(), 37);
    for (std::size_t index = 0; index < destination3.size(); ++index) {
        EXPECT_EQ(destination3[index], (index + 1) * 10);
    }

    destination3 = std::move(source3);
    EXPECT_EQ(destination3.size(), 6);
    EXPECT_EQ(destination3.capacity(), 6);
    EXPECT_NE(destination3.data(), nullptr);
    EXPECT_EQ(destination3.get_allocator().id(), 37);
    for (std::size_t index = 0; index < destination3.size(); ++index) {
        EXPECT_EQ(destination3[index], ((index + 1) * 10 + index));
    }

    EXPECT_EQ(source3.size(), 0);
    EXPECT_EQ(source3.capacity(), 0);
    EXPECT_EQ(source3.data(), nullptr);
    EXPECT_EQ(source3.get_allocator().id(), 73);
}

TEST(VectorTests, TrackingIntAllocatorMoveAssignmentOperator)
{
    TrackingIntVector source{10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    EXPECT_EQ(source.size(), 10);
    EXPECT_EQ(source.capacity(), 10);
    EXPECT_NE(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);
    for (std::size_t index = 0; index < source.size(); ++index) {
        EXPECT_EQ(source[index], (index + 1) * 10);
    }

    TrackingIntVector destination{1, 2, 3};
    EXPECT_EQ(destination.size(), 3);
    EXPECT_EQ(destination.capacity(), 3);
    EXPECT_NE(destination.data(), nullptr);
    EXPECT_EQ(destination.get_allocator().id(), 42);
    for (std::size_t index = 0; index < destination.size(); ++index) {
        EXPECT_EQ(destination[index], index + 1);
    }

    destination = std::move(source);
    EXPECT_EQ(destination.size(), 10);
    EXPECT_EQ(destination.capacity(), 10);
    EXPECT_NE(destination.data(), nullptr);
    EXPECT_EQ(destination.get_allocator().id(), 42);
    for (std::size_t index = 0; index < destination.size(); ++index) {
        EXPECT_EQ(destination[index], (index + 1) * 10);
    }

    EXPECT_EQ(source.size(), 0);
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(source.get_allocator().id(), 42);

    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    EXPECT_NE(pRegistry, nullptr);
    TrackingIntAllocator allocator(91, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator.id());
    {
        TrackingIntVector source2({10, 21, 32, 43, 54, 65}, allocator);
        EXPECT_EQ(source2.size(), 6);
        EXPECT_EQ(source2.capacity(), 6);
        EXPECT_NE(source2.data(), nullptr);
        EXPECT_EQ(source2.get_allocator().id(), 91);
        for (std::size_t index = 0; index < source2.size(); ++index) {
            EXPECT_EQ(source2[index], ((index + 1) * 10 + index));
        }

        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 6);
        EXPECT_EQ(stats.destructions, 0);

        TrackingIntVector destination2(20, 13, allocator);
        EXPECT_EQ(destination2.size(), 20);
        EXPECT_EQ(destination2.capacity(), 20);
        EXPECT_NE(destination2.data(), nullptr);
        EXPECT_EQ(destination2.get_allocator().id(), 91);
        for (std::size_t index = 0; index < destination2.size(); ++index) {
            EXPECT_EQ(destination2[index], 13);
        }

        EXPECT_EQ(stats.allocations, 2);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 26);
        EXPECT_EQ(stats.destructions, 0);

        destination2 = std::move(source2);
        EXPECT_EQ(destination2.size(), 6);
        EXPECT_EQ(destination2.capacity(), 6);
        EXPECT_NE(destination2.data(), nullptr);
        EXPECT_EQ(destination2.get_allocator().id(), 91);
        for (std::size_t index = 0; index < destination2.size(); ++index) {
            EXPECT_EQ(destination2[index], ((index + 1) * 10 + index));
        }

        EXPECT_EQ(source2.size(), 0);
        EXPECT_EQ(source2.capacity(), 0);
        EXPECT_EQ(source2.data(), nullptr);
        EXPECT_EQ(source2.get_allocator().id(), 91);

        EXPECT_EQ(stats.allocations, 2);
        EXPECT_EQ(stats.deallocations, 1);
        EXPECT_EQ(stats.constructions, 26);
        EXPECT_EQ(stats.destructions, 20);
    }

    EXPECT_EQ(stats.allocations, 2);
    EXPECT_EQ(stats.deallocations, 2);
    EXPECT_EQ(stats.constructions, 26);
    EXPECT_EQ(stats.destructions, 26);


    TrackingIntAllocator srcAllocator(73, pRegistry);
    const AllocationStats& srcStats = pRegistry->statsFor(srcAllocator.id());
    TrackingIntAllocator destAllocator(37, pRegistry);
    const AllocationStats& destStats = pRegistry->statsFor(destAllocator.id());
    {
        TrackingIntVector source3({10, 21, 32, 43, 54, 65}, srcAllocator);
        EXPECT_EQ(source3.size(), 6);
        EXPECT_EQ(source3.capacity(), 6);
        EXPECT_NE(source3.data(), nullptr);
        EXPECT_EQ(source3.get_allocator().id(), 73);
        for (std::size_t index = 0; index < source3.size(); ++index) {
            EXPECT_EQ(source3[index], ((index + 1) * 10 + index));
        }

        EXPECT_EQ(srcStats.allocations, 1);
        EXPECT_EQ(srcStats.deallocations, 0);
        EXPECT_EQ(srcStats.constructions, 6);
        EXPECT_EQ(srcStats.destructions, 0);

        TrackingIntVector destination3({10, 20, 30, 40, 50, 60, 70, 80, 90, 100}, destAllocator);
        EXPECT_EQ(destination3.size(), 10);
        EXPECT_EQ(destination3.capacity(), 10);
        EXPECT_NE(destination3.data(), nullptr);
        EXPECT_EQ(destination3.get_allocator().id(), 37);
        for (std::size_t index = 0; index < destination3.size(); ++index) {
            EXPECT_EQ(destination3[index], (index + 1) * 10);
        }

        EXPECT_EQ(destStats.allocations, 1);
        EXPECT_EQ(destStats.deallocations, 0);
        EXPECT_EQ(destStats.constructions, 10);
        EXPECT_EQ(destStats.destructions, 0);

        destination3 = std::move(source3);
        EXPECT_EQ(destination3.size(), 6);
        EXPECT_EQ(destination3.capacity(), 6);
        EXPECT_NE(destination3.data(), nullptr);
        EXPECT_EQ(destination3.get_allocator().id(), 37);
        for (std::size_t index = 0; index < destination3.size(); ++index) {
            EXPECT_EQ(destination3[index], ((index + 1) * 10 + index));
        }

        EXPECT_EQ(source3.size(), 0);
        EXPECT_EQ(source3.capacity(), 0);
        EXPECT_EQ(source3.data(), nullptr);
        EXPECT_EQ(source3.get_allocator().id(), 73);

        EXPECT_EQ(srcStats.allocations, 1);
        EXPECT_EQ(srcStats.deallocations, 1);
        EXPECT_EQ(srcStats.constructions, 6);
        EXPECT_EQ(srcStats.destructions, 6);

        EXPECT_EQ(destStats.allocations, 2);
        EXPECT_EQ(destStats.deallocations, 1);
        EXPECT_EQ(destStats.constructions, 16);
        EXPECT_EQ(destStats.destructions, 10);
    }

    EXPECT_EQ(srcStats.allocations, 1);
    EXPECT_EQ(srcStats.deallocations, 1);
    EXPECT_EQ(srcStats.constructions, 6);
    EXPECT_EQ(srcStats.destructions, 6);

    EXPECT_EQ(destStats.allocations, 2);
    EXPECT_EQ(destStats.deallocations, 2);
    EXPECT_EQ(destStats.constructions, 16);
    EXPECT_EQ(destStats.destructions, 16);
}
