# Vulxels

Vulxels is/will be a voxel-based game written in C++ using the Vulkan API. The goal of this project is ultimately to learn more about low-level graphics programming and performance optimization, while also creating a fun Minecraft-like game. The name "Vulxels" is a portmanteau of "Vulkan" and "voxels".

## Building

### Dependencies

- [Vulkan](https://vulkan.lunarg.com/sdk/home)
- [SDL3](https://github.com/libsdl-org/SDL)
- [GLM](https://github.com/g-truc/glm)
- [EnTT](https://github.com/skypjack/entt)

### Instructions

1. Clone the repository:
```bash
git clone https://github.com/JaydenGrubb1/vulxels.git
```
2. Make build directory:
```bash
mkdir build && cd build
```
3. Run CMake:
```bash
cmake ../
```
4. Build:
```bash
make
```
5. Run:
```bash
./vulxels
```
