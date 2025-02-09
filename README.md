# cubana
**The project is currently being developed.**

Cubana is a simple 3D game written from scratch in C with a vulkan renderer. The aim of the game is for the player to escape the labirynth by solving puzzles, avoiding enemies and advancing through the maze. 

C has been chosen for its simplicity, fast compile times and ease of debugability. Vulkan has been chosen as a go-to API due to its C-API interface and lack of OpenGL's weirdness.
Due to expected low-performance footprint and to keep the code simple, the game is designed to run single-threaded.

One of the main principles standing behind the philosophy of developing cubana is creating all major components from scratch. Hence the usage of external code has been minimised to:
- SDL - used for an OS-layer abstraction
- lua - scripting game logic and interactive debug
The reasons are: 1) easier debuggability 2) better control over the performance 3) preference for solutions crafted to the problem at stake and not _similar_ problems and 4) desire for a broad-ranging understanding of how all the components work together to achieve the goal of creating an immersive and fun to engage in virtual 3D-world.

The exception to this rule are tools and third-party libraries used during development process only and are typically used for better debugging experience and won't be used in the final game.

# Current state
Before the actual game will begin its development, it is important to lay foundations that will make the development a smooth and pleasureable experience.
This includes creation of basic utilities, data structures, memory allocators, debugging tools, platform abstraction layer, I/O handling and others.
Currently, the following features are present in the engine:
- Basic cube rendering in arbitrary positions, sizes and colours (using a Vulkan backend)
- Debug UI (thanks to ImGUI and C/lua bindings, currently being reworked)
- In-game script editing and execution with lua
- Basic 3D primitives drawing
- Logging system
- Memory allocators (to be reworked)
- Basic platform abstraction layer
- Keyboard and GamePad input handling
- Basic event system
- Save/Load and Replay systems
- Entity system based on megastruct philosophy (at least for now)
- AABB collision detection
- Simple AABB-based physics engine
- Level loading from .ctm files

![image](https://github.com/user-attachments/assets/ecbea400-3fe5-4b5c-b8f2-7fac0d2cee70)


# Development
In the nearest fututre, I'll be working on:
- Creating a Trigger entity for launching a chosen lua script upon collision
- Creating a Camera following player
- Making lua scripts hot-reloadable
- Preventing whole-program crashes on lua errors

## Building
```shell
mkdir build
cd build
cmake ..
cmake --build . -j
```
