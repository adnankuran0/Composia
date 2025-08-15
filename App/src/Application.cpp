#include "Application.h"

#include "Composia.h"
#include <vector>
#include <iostream>
#include <chrono>

using namespace Composia;

void Benchmark();

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

void Application::Start()
{
    {
        Registry reg;
        Entity ent1 = reg.Create();
        Entity ent2 = reg.Create();

        reg.Emplace<Position>(ent1, 10.0f, 25.0f);
        reg.Emplace<Velocity>(ent1, 10.0f, 2.0f);

        reg.Emplace<Position>(ent2, 42.0f, 21.0f);
        reg.Emplace<Velocity>(ent2, 21.0f, 9.0f);

        // Iterate using View
        reg.View<Position, Velocity>().each([](Position& pos, Velocity& vel) {
            std::cout << "Entity with Position(" << pos.x << ", " << pos.y
                << ") and Velocity(" << vel.x << ", " << vel.y << ")\n";
            });

        std::cout << "Has ent1 Position? : " << reg.Has<Position>(ent1) << "\n";
        reg.Remove<Position>(ent1);
        std::cout << "Has ent1 Position after removing it? : " << reg.Has<Position>(ent1) << "\n";

        reg.Destroy(ent2);
        std::cout << "Has ent2 Velocity after destroyed? : " << reg.Has<Velocity>(ent2) << "\n";
    }

    Benchmark();
}

void Benchmark()
{
    std::cout << "\n-----------------Benchmark------------------\n";

    using Clock = std::chrono::high_resolution_clock;
    Registry registry;

    constexpr int entityCount = 100000;

    // Entity create benchmark
    auto start = Clock::now();
    for (int i = 0; i < entityCount; ++i)
        registry.Create();
    auto end = Clock::now();
    std::cout << "Create " << entityCount << " entities: "
        << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    // Emplace components benchmark
    Composia::Core::DynamicArray<Entity> entities;
    entities.Reserve(entityCount);
    for (int i = 0; i < entityCount; ++i)
        entities.PushBack(registry.Create());

    start = Clock::now();
    for (auto e : entities)
        registry.Emplace<Position>(e, 0.f, 0.f);
    end = Clock::now();
    std::cout << "Emplace Position: "
        << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = Clock::now();
    for (auto e : entities)
        registry.Emplace<Velocity>(e, 1.f, 1.f);
    end = Clock::now();
    std::cout << "Emplace Velocity: "
        << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";


    // Entity destroy benchmark
    start = Clock::now();
    for (auto e : entities)
        registry.Destroy(e);
    end = Clock::now();
    std::cout << "Destroy " << entityCount << " entities: "
        << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
}