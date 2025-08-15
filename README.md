# Composia ECS

Composia is a small, fast, header-only Entity Component System (ECS) library.
It can be used as a single header via Composia.h or with separate modular headers. 

## Code Example
```cpp
#include "Composia.h"
#include <iostream>

using namespace Composia;

struct Position { float x, y; };
struct Velocity { float x, y; };

int main() {
    Registry registy;
    Entity entity1 = registy.Create();
    Entity entity2 = registy.Create();

    registy.Emplace<Position>(entity1, 10.0f, 25.0f);
    registy.Emplace<Velocity>(entity1, 5.0f, 2.0f);

    registy.Emplace<Position>(entity2, 42.0f, 21.0f);
    registy.Emplace<Velocity>(entity2, 21.0f, 9.0f);

    registy.View<Position, Velocity>().each([](Position& pos, Velocity& vel) {
        std::cout << "Position(" << pos.x << ", " << pos.y
                  << "), Velocity(" << vel.x << ", " << vel.y << ")\n";
    });
}
```
## Getting Started

### Installation

To get the project up and running, you'll need to follow the setup instructions for your platform.

#### For Windows
1. Navigate to the `Scripts` folder in the project directory.
2. Run the Setup-Windows.bat batch file.

#### For Linux

1. Navigate to the `Scripts` folder in the project directory.

2. Make the `Setup-Linux.sh` script executable:
   
   ```bash
   chmod +x Setup-Linux.sh

3. Run the script to install necessary dependencies and set up the project:
   ```bash
   ./Setup-Linux.sh

