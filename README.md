# 2D Game Engine (Pikuma) 🧱

A robust 2D tile-based engine built with **C++** and **SDL2**, focusing on the bridge between low-level performance and high-level scripting flexibility.

## 🚀 Engine Architecture
* **Lua Scripting Integration**: Full scripting bridge allowing game logic to be written in Lua, enabling rapid iteration without C++ recompilation.
* **Tilemap System**: Automated loading and rendering of complex 2D maps exported from Tiled (JSON format).
* **Asset Management**: Centralized `AssetManager` for optimized texture, font, and sound loading.
* **Game Loop**: Implementation of a **Fixed Timestep** loop to ensure deterministic physics and smooth rendering across different hardware.
* **Custom ECS**: A modular entity-component system for decoupled game object logic.

## 📚 Acknowledgments
Developed during the "2D Game Engine Programming" course by **Pikuma (Gustavo Peixoto)**. This project served as a deep dive into memory management and the internal mechanics of game engines.

## 🛠 Tech Stack
* **Language:** C++, Lua
* **API:** SDL2, SDL_image, SDL_ttf, SDL_mixer
