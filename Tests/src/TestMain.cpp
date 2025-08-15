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
#include <ComponentManager.h>
#include <Registry.h>
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

// -------------------------
// ComponentManager basic tests
// -------------------------

struct Position { int x, y; };
struct Velocity { float vx, vy; };

class ComponentManagerTest : public ::testing::Test {
protected:
    EntityManager entityManager;
    ComponentManager compManager;
};

TEST_F(ComponentManagerTest, AddAndGetComponent)
{
    Entity e = entityManager.Create();
    Position pos{ 10, 20 };
    compManager.Add(e, pos);

    auto* retrieved = compManager.Get<Position>(e);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->x, 10);
    EXPECT_EQ(retrieved->y, 20);
}

TEST_F(ComponentManagerTest, EmplaceComponent)
{
    Entity e = entityManager.Create();
    compManager.Emplace<Velocity>(e, 1.5f, 2.5f);

    auto* v = compManager.Get<Velocity>(e);
    ASSERT_NE(v, nullptr);
    EXPECT_FLOAT_EQ(v->vx, 1.5f);
    EXPECT_FLOAT_EQ(v->vy, 2.5f);
}

TEST_F(ComponentManagerTest, RemoveComponent)
{
    Entity e = entityManager.Create();
    compManager.Add(e, Position{ 5, 6 });
    EXPECT_TRUE(compManager.Has<Position>(e));

    compManager.Remove<Position>(e);
    EXPECT_FALSE(compManager.Has<Position>(e));
    EXPECT_EQ(compManager.Get<Position>(e), nullptr);
}

TEST_F(ComponentManagerTest, HasComponent)
{
    Entity e = entityManager.Create();
    EXPECT_FALSE(compManager.Has<Velocity>(e));

    compManager.Add(e, Velocity{ 3.f, 4.f });
    EXPECT_TRUE(compManager.Has<Velocity>(e));
}

TEST_F(ComponentManagerTest, PoolAccess)
{
    Entity e = entityManager.Create();
    compManager.Add(e, Position{ 7, 8 });

    auto* pool = compManager.Pool<Position>();
    ASSERT_NE(pool, nullptr);
    EXPECT_TRUE(pool->Has(e));
}

TEST_F(ComponentManagerTest, RemoveAllForEntity)
{
    Entity e = entityManager.Create();
    compManager.Add(e, Position{ 1,2 });
    compManager.Add(e, Velocity{ 3.f,4.f });

    compManager.RemoveAllForEntity(e);

    EXPECT_FALSE(compManager.Has<Position>(e));
    EXPECT_FALSE(compManager.Has<Velocity>(e));
    EXPECT_EQ(compManager.Get<Position>(e), nullptr);
    EXPECT_EQ(compManager.Get<Velocity>(e), nullptr);
}

TEST_F(ComponentManagerTest, MultipleEntities)
{
    Entity e1 = entityManager.Create();
    Entity e2 = entityManager.Create();

    compManager.Add(e1, Position{ 1,1 });
    compManager.Add(e2, Position{ 2,2 });

    EXPECT_TRUE(compManager.Has<Position>(e1));
    EXPECT_TRUE(compManager.Has<Position>(e2));

    compManager.Remove<Position>(e1);
    EXPECT_FALSE(compManager.Has<Position>(e1));
    EXPECT_TRUE(compManager.Has<Position>(e2));
}

// -------------------------
// Registry tests
// -------------------------

class RegistryTest : public ::testing::Test 
{
protected:
    Registry registry;
};

TEST_F(RegistryTest, AddAndGetComponent) 
{
    Entity e = registry.Create();
    Position pos{ 10, 20 };
    registry.Add(e, pos);

    Position& got = registry.Get<Position>(e);
    EXPECT_EQ(got.x, 10);
    EXPECT_EQ(got.y, 20);
}

TEST_F(RegistryTest, EmplaceComponent) 
{
    Entity e = registry.Create();
    registry.Emplace<Velocity>(e, 1.0f, 2.0f);

    Velocity& vel = registry.Get<Velocity>(e);
    EXPECT_FLOAT_EQ(vel.vx, 1.0f);
    EXPECT_FLOAT_EQ(vel.vy, 2.0f);
}

TEST_F(RegistryTest, RemoveComponent)
{
    Entity e = registry.Create();
    registry.Emplace<Position>(e, 5, 6);
    registry.Remove<Position>(e);

    EXPECT_FALSE(registry.Has<Position>(e));
}

TEST_F(RegistryTest, DestroyEntity_RemovesAllComponents) 
{
    Entity e = registry.Create();
    registry.Emplace<Position>(e, 1, 2);
    registry.Emplace<Velocity>(e, 3.0f, 4.0f);

    registry.Destroy(e);

    EXPECT_FALSE(registry.Has<Position>(e));
    EXPECT_FALSE(registry.Has<Velocity>(e));
}

TEST_F(RegistryTest, MultipleEntities) 
{
    Entity e1 = registry.Create();
    Entity e2 = registry.Create();

    registry.Emplace<Position>(e1, 10, 10);
    registry.Emplace<Position>(e2, 20, 20);

    Position& p1 = registry.Get<Position>(e1);
    Position& p2 = registry.Get<Position>(e2);

    EXPECT_EQ(p1.x, 10);
    EXPECT_EQ(p2.x, 20);
}


int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

