#include <gtest/gtest.h>
#include "Core/DynamicArray.h"

using namespace Composia::Core;

// -------------------------
// DynamicArray<int> Tests
// -------------------------

TEST(DynamicArrayTest, InitialState) 
{
    DynamicArray<int> arr;
    EXPECT_EQ(arr.Size(), 0);
    EXPECT_GE(arr.Capacity(), 1);
    EXPECT_NE(arr.Data(), nullptr);
}

TEST(DynamicArrayTest, PushBackIncreasesSize) 
{
    DynamicArray<int> arr;
    arr.PushBack(10);
    arr.PushBack(20);
    EXPECT_EQ(arr.Size(), 2);
    EXPECT_EQ(arr.At(0), 10);
    EXPECT_EQ(arr.At(1), 20);
}

TEST(DynamicArrayTest, PushBackAndPopBack) 
{
    DynamicArray<int> arr;
    arr.PushBack(1);
    arr.PushBack(2);
    arr.PushBack(3);
    arr.PopBack();
    EXPECT_EQ(arr.Size(), 2);
    EXPECT_EQ(arr.At(0), 1);
    EXPECT_EQ(arr.At(1), 2);
}

TEST(DynamicArrayTest, ResizeIncreasesSizeWithDefaultCtor) 
{
    DynamicArray<int> arr;
    arr.Resize(5);
    EXPECT_EQ(arr.Size(), 5);
    for (size_t i = 0; i < 5; i++)
        EXPECT_EQ(arr.At(i), 0); // int default is 0
}

TEST(DynamicArrayTest, ResizeDecreasesSize) 
{
    DynamicArray<int> arr;
    arr.Resize(5);
    arr.Resize(2);
    EXPECT_EQ(arr.Size(), 2);
}

TEST(DynamicArrayTest, ResizeWithValue) 
{
    DynamicArray<int> arr;
    arr.Resize(3, 42);
    EXPECT_EQ(arr.Size(), 3);
    for (size_t i = 0; i < 3; i++)
        EXPECT_EQ(arr.At(i), 42);
}

TEST(DynamicArrayTest, ReserveIncreasesCapacity) 
{
    DynamicArray<int> arr;
    size_t oldCapacity = arr.Capacity();
    arr.Reserve(oldCapacity * 2);
    EXPECT_GE(arr.Capacity(), oldCapacity * 2);
}

TEST(DynamicArrayTest, AtThrowsOnInvalidIndex) 
{
    DynamicArray<int> arr;
    arr.PushBack(5);
    // Should trigger assert if compiled with debug
    EXPECT_DEATH({
        arr.At(1);
        }, "Index out of bounds");
}

TEST(DynamicArrayTest, GrowWorksAutomatically) 
{
    DynamicArray<int> arr(1);
    size_t initialCapacity = arr.Capacity();
    arr.PushBack(1);
    arr.PushBack(2);
    EXPECT_GT(arr.Capacity(), initialCapacity);
    EXPECT_EQ(arr.Size(), 2);
    EXPECT_EQ(arr.At(0), 1);
    EXPECT_EQ(arr.At(1), 2);
}

TEST(DynamicArrayTest, Back)
{
    DynamicArray<int> arr(1);
    arr.PushBack(1);
    arr.PushBack(2);
    EXPECT_EQ(arr.Back(), 2);
}

TEST(DynamicArrayTest, Front)
{
    DynamicArray<int> arr(1);
    arr.PushBack(1);
    arr.PushBack(2);
    EXPECT_EQ(arr.Front(), 1);
}

TEST(DynamicArrayTest, Empty)
{
    DynamicArray<int> arr(1);
    arr.PushBack(1);
    arr.PushBack(2);
    EXPECT_EQ(arr.Empty(), false);
    arr.PopBack();
    arr.PopBack();
    EXPECT_EQ(arr.Empty(), true);
}

// -------------------------
// DynamicArray<std::string> Tests
// -------------------------
#include <string>

TEST(DynamicArrayStringTest, PushBackString) 
{
    DynamicArray<std::string> arr;
    arr.PushBack("Hello");
    arr.PushBack("World");
    EXPECT_EQ(arr.Size(), 2);
    EXPECT_EQ(arr.At(0), "Hello");
    EXPECT_EQ(arr.At(1), "World");
}

TEST(DynamicArrayStringTest, ResizeWithStringValue) 
{
    DynamicArray<std::string> arr;
    arr.Resize(3, "Test");
    EXPECT_EQ(arr.Size(), 3);
    for (size_t i = 0; i < 3; i++)
        EXPECT_EQ(arr.At(i), "Test");
}

TEST(DynamicArrayStringTest, ClearAllValues)
{
    DynamicArray<std::string> arr;
    arr.PushBack("Hello");
    arr.PushBack("World");
    arr.Clear();
    EXPECT_EQ(arr.Size(), 0);
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}