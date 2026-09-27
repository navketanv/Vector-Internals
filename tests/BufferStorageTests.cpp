#include <gtest/gtest.h>
#include <utility>
#include <memory>

#include "Memory/Allocator.h"
#include "Memory/StatefulAllocator.h"
#include "Memory/BufferStorage.h"
#include "Memory/TrackingAllocator.h"
#include "Memory/AllocationRegistry.h"
#include "Memory/AllocationStats.h"

// BufferStorageTests.cpp
using TrackingIntAllocator = TrackingAllocator<int>;
using BufferStorageIntTracking = BufferStorage<int, TrackingIntAllocator>;

TEST(BufferStorageTests, ZeroCapacityConstruction)
{
    Allocator<int> allocator{};
    BufferStorage<int> storage(allocator, 0);
    EXPECT_EQ(storage.data(), nullptr);
    EXPECT_EQ(storage.capacity(), 0);
    EXPECT_FALSE(storage.hasStorage());
}

TEST(BufferStorageTests, CapacityConstruction)
{
    BufferStorage<int> storage(10);
    EXPECT_NE(storage.data(), nullptr);
    EXPECT_EQ(storage.capacity(), 10);
    EXPECT_TRUE(storage.hasStorage());
}

TEST(BufferStorageTests, AllocatorConstruction)
{
    Allocator<int> allocator{};
    BufferStorage<int, Allocator<int>> storage(allocator, 10);
    EXPECT_NE(storage.data(), nullptr);
    EXPECT_EQ(storage.capacity(), 10);
    EXPECT_TRUE(storage.hasStorage());
}

TEST(BufferStorageTests, MoveConstruction)
{
    Allocator<int> allocator{};
    BufferStorage<int, Allocator<int>> source(allocator, 10);
    int* sourceData = source.data();

    BufferStorage<int, Allocator<int>> destination(std::move(source));
    EXPECT_EQ(sourceData, destination.data());
    EXPECT_EQ(destination.capacity(), 10);
    EXPECT_TRUE(destination.hasStorage());

    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_FALSE(source.hasStorage());
}

TEST(BufferStorageTests, MoveAssignment)
{
    Allocator<int> allocator{};
    BufferStorage<int, Allocator<int>> source(allocator, 10);
    BufferStorage<int, Allocator<int>> destination(allocator, 5);

    EXPECT_TRUE(AllocatorPolicy<Allocator<int>>::allocators_compatible(source.allocator(), destination.allocator()));

    int* sourceData = source.data();
    destination = std::move(source);
    EXPECT_EQ(destination.data(), sourceData);
    EXPECT_EQ(destination.capacity(), 10);
    EXPECT_TRUE(destination.hasStorage());

    EXPECT_EQ(source.data(), nullptr);
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_FALSE(source.hasStorage());
}

TEST(BufferStorageTests, StatefulAllocatorConstruction)
{
    StatefulAllocator<int> allocator{42};
    BufferStorage<int, StatefulAllocator<int>> storage(allocator, 10);
    EXPECT_EQ(allocator.id(), 42);
    EXPECT_EQ(storage.allocator().id(), allocator.id());
    EXPECT_EQ(storage.capacity(), 10);
    EXPECT_NE(storage.data(), nullptr);
    EXPECT_TRUE(storage.hasStorage());
}

TEST(BufferStorageTests, StatefulAllocatorMoveConstruction)
{
    StatefulAllocator<int> allocator{42};
    BufferStorage<int, StatefulAllocator<int>> source(allocator, 10);
    int* sourceData = source.data();
    BufferStorage<int, StatefulAllocator<int>> destination(std::move(source));

    EXPECT_EQ(allocator.id(), 42);

    EXPECT_EQ(destination.data(), sourceData);
    EXPECT_EQ(destination.allocator().id(), allocator.id());
    EXPECT_EQ(destination.capacity(), 10);
    EXPECT_NE(destination.data(), nullptr);
    EXPECT_TRUE(destination.hasStorage());

    EXPECT_EQ(source.allocator().id(), allocator.id());
    EXPECT_EQ(source.capacity(), 0);
    EXPECT_EQ(source.data(), nullptr);
    EXPECT_FALSE(source.hasStorage());
}

TEST(BufferStorageTests, FactoryConstructsStatefulAllocator)
{
    BufferStorage<int, StatefulAllocator<int>> storage(10);

    EXPECT_EQ(storage.allocator().id(), 42);
    EXPECT_EQ(storage.capacity(), 10);
    EXPECT_NE(storage.data(), nullptr);
    EXPECT_TRUE(storage.hasStorage());
}

TEST(BufferStorageTests, FactoryConstructsTrackingAllocator)
{
    BufferStorage<int, TrackingAllocator<int>> storage(10);

    EXPECT_EQ(storage.allocator().id(), 42);
    EXPECT_EQ(storage.capacity(), 10);
    EXPECT_NE(storage.data(), nullptr);
    EXPECT_TRUE(storage.hasStorage());
}

TEST(BufferStorageTests, TrackingAllocatorTracksAllocationDeallocation)
{
    std::shared_ptr<AllocationRegistry> pRegistry = std::make_shared<AllocationRegistry>();
    EXPECT_NE(pRegistry, nullptr);
    TrackingIntAllocator allocator(12, pRegistry);
    const AllocationStats& stats = pRegistry->statsFor(allocator.id());
    {
        BufferStorageIntTracking storage(allocator, 128);

        EXPECT_EQ(storage.allocator().id(), 12);
        EXPECT_EQ(storage.capacity(), 128);
        EXPECT_NE(storage.data(), nullptr);
        EXPECT_TRUE(storage.hasStorage());
        EXPECT_EQ(stats.allocations, 1);
        EXPECT_EQ(stats.deallocations, 0);
        EXPECT_EQ(stats.constructions, 0);
        EXPECT_EQ(stats.destructions, 0);
    }
    EXPECT_EQ(stats.allocations, 1);
    EXPECT_EQ(stats.deallocations, 1);
    EXPECT_EQ(stats.constructions, 0);
    EXPECT_EQ(stats.destructions, 0);
}
