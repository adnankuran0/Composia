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

TEST(DynamicArrayTest, Foreach)
{
    DynamicArray<int> arr(1);
    arr.PushBack(1);
    arr.PushBack(2);
    arr.PushBack(3);
    arr.PushBack(4);
    arr.PushBack(5);
    int foundValue;
    for (auto& val : arr) {
        if (val == 5) foundValue = 5;
    }
    EXPECT_EQ(foundValue, 5);
}

TEST(DynamicArrayTest, STDFind)
{
    DynamicArray<int> arr(1);
    arr.PushBack(1);
    arr.PushBack(2);
    arr.PushBack(3);
    arr.PushBack(4);
    arr.PushBack(5);

    auto it = std::find(arr.begin(), arr.end(), 3);
    bool result = it != arr.end();
    EXPECT_EQ(result, true);
    it = std::find(arr.begin(), arr.end(), 10);
    result = it != arr.end();
    EXPECT_EQ(result, false);
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

// -------------------------
// Entity and EntityManager tests
// -------------------------

#include "EntityManager.h"
using namespace Composia;

class EntityManagerTest : public ::testing::Test {
protected:
    EntityManager manager;
};

TEST_F(EntityManagerTest, Create_NewEntity_ShouldBeAlive)
{
    Entity e = manager.Create();
    EXPECT_TRUE(manager.IsAlive(e));
    EXPECT_EQ(manager.Generation(e), 0);
}

TEST_F(EntityManagerTest, Create_MultipleEntities_UniqueIDs)
{
    Entity e1 = manager.Create();
    Entity e2 = manager.Create();
    EXPECT_NE(e1, e2);
    EXPECT_TRUE(manager.IsAlive(e1));
    EXPECT_TRUE(manager.IsAlive(e2));
}

TEST_F(EntityManagerTest, Destroy_Entity_ShouldNotBeAlive)
{
    Entity e = manager.Create();
    manager.Destroy(e);
    EXPECT_FALSE(manager.IsAlive(e));
}

TEST_F(EntityManagerTest, Destroy_AlreadyDestroyedEntity_NoCrash)
{
    Entity e = manager.Create();
    manager.Destroy(e);
    EXPECT_NO_THROW(manager.Destroy(e));
    EXPECT_FALSE(manager.IsAlive(e));
}

TEST_F(EntityManagerTest, Create_AfterDestroy_ReusesID)
{
    Entity e1 = manager.Create();
    manager.Destroy(e1);
    Entity e2 = manager.Create();
    EXPECT_EQ(e1, e2);
    EXPECT_TRUE(manager.IsAlive(e2));
}

TEST_F(EntityManagerTest, Generation_ShouldIncreaseWhenReused)
{
    Entity e1 = manager.Create();
    uint32_t gen1 = manager.Generation(e1);
    manager.Destroy(e1);
    Entity e2 = manager.Create();
    uint32_t gen2 = manager.Generation(e2);
    EXPECT_EQ(e1, e2);
    EXPECT_EQ(gen2, gen1 + 1);
}

TEST_F(EntityManagerTest, IsAlive_InvalidEntity_ShouldReturnFalse)
{
    EXPECT_FALSE(manager.IsAlive(999));
}

TEST_F(EntityManagerTest, Generation_InvalidEntity_ShouldReturnZero)
{
    EXPECT_EQ(manager.Generation(999), 0);
}
int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

