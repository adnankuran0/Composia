#include "Application.h"

#include "Composia.h"
#include <vector>
#include <iostream>
#include <chrono>

using namespace Composia;

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
    Registry reg;
    Entity ent1 = reg.Create();
    Entity ent2 = reg.Create();

    Position pos = { 10.0f , 25.0f };
    reg.Add<Position>(ent1, pos);
    reg.Emplace<Velocity>(ent1,10.0f,2.0f);

    reg.Emplace<Position>(ent2, 42.0f, 21.0f);
    reg.Emplace<Velocity>(ent2, 21.0f, 9.0f);

    pos = reg.Get<Position>(ent1);
    Velocity vel = reg.Get<Velocity>(ent2);


    std::cout << "Position of ent1: " << pos.x << "," << pos.y << "\n";
    std::cout << "Velocity of ent2: " << vel.x << "," << vel.y << "\n";

    std::cout << "Has ent1 Position component? : " << reg.Has<Position>(ent1) << "\n";
    reg.Remove<Position>(ent1);
    std::cout << "Has ent1 Position component after removing it? : " << reg.Has<Position>(ent1) << "\n";

    reg.Destroy(ent2);
    std::cout << "Has ent2 Velocity component after destroyed? : " << reg.Has<Velocity>(ent2) << "\n";

}