# AGENTS.md

This file provides guidance to AI agents when working with code in this repository.

## Project Overview

CppSdl3 is a C++ wrapper around SDL3 (Simple Direct Media Layer) using SDL_gpu together with ImGui. It provides a modern C++23 interface for creating GPU-accelerated applications with integrated Dear ImGui support.

The library is designed to be consumed via CMake's FetchContent, providing a clean API where users inherit from `sdl::Window` and override virtual methods for rendering and event handling.

## Build System

### Prerequisites
- CMake 4.2 or higher
- C++23 compiler (MSVC, GCC, or Clang)
- DirectX Shader Compiler (dxc) for shader compilation
- vcpkg

### Building

#### Windows
```bash
cmake --preset=windows -B build -DCppSdl3_Example=1 -DCppSdl3_Test=1
cmake --build build
```

#### Unix/Linux
```bash
cmake --preset=unix -B build -DCppSdl3_Example=1 -DCppSdl3_Test=1
cmake --build build
```

### Running Tests
Tests use GoogleTest with CTest integration:
```bash
cd build
ctest
# Or run the test executable directly
./CppSdl3_Test/CppSdl3_Test  # Unix
.\CppSdl3_Test\Debug\CppSdl3_Test.exe  # Windows
```

## Architecture

All files is named using lowercase. And any new file must be added to corresponding CMakeLists.txt.

Graphic is done by SDL_gpu.

### Core Components

**sdl::Window** ([src/sdl/window.h](src/sdl/window.h), [src/sdl/window.cpp](src/sdl/window.cpp))
- The central class that users inherit from to create applications
- Manages the SDL window, GPU device, ImGui context, and main event loop
- Key virtual methods to override:
  - `renderImGui(const DeltaTime&)` - Render ImGui UI elements
  - `processEvent(const SDL_Event&)` - Handle SDL events
  - `renderFrame(const DeltaTime&, SDL_GPUTexture*, SDL_GPUCommandBuffer*)` - Custom GPU rendering before ImGui
  - `preLoop()` / `postLoop()` - Setup/cleanup hooks
- Call `startLoop()` to begin the main loop; call `quit()` to exit

**GPU Resource Management** ([src/sdl/gpu.h](src/sdl/gpu.h))
- RAII wrappers for SDL_gpu resources using `std::unique_ptr` with custom deleters
- Type aliases: `GpuTexture`, `GpuBuffer`, `GpuShader`, `GpuGraphicsPipeline`, etc.
- Factory functions: `createGpuTexture()`, `createGpuBuffer()`, etc.
- All GPU resources automatically track their associated `SDL_GPUDevice` for proper cleanup
- Helper utilities: `gpuCopyPass()`, `mapGpuTransferBuffer()`
- `VertexType` concept validates vertex struct requirements

**ImGui Integration** ([ImGui/CMakeLists.txt](ImGui/CMakeLists.txt))
- Fetches Dear ImGui from ocornut/imgui repository
- Configured with SDL3 and SDL_gpu backends
- FreeType support enabled for font rendering
- Custom config via `ImGui/src/imconfig.h`

**Shader System** ([src/sdl/shader.h](src/sdl/shader.h), [compile_shaders.py](compile_shaders.py))
- Shaders are written in HLSL and compiled to both DXIL and SPIR-V
- Use `compile_shaders.py` to compile shaders and generate C++ headers with embedded bytecode
- Generated headers contain `constexpr std::array<uint8_t>` for each shader variant
- Example: `shader.vs.hlsl` → `shader.vs.h` with `ShaderVsDxilBytes` and `ShaderVsSpirvBytes`

**Utilities**
- [src/sdl/util.h](src/sdl/util.h) - `DeltaTime`, `Buffer` helper for GPU buffer management
- [src/sdl/color.h](src/sdl/color.h) - Color type and utilities
- [src/sdl/batch.h](src/sdl/batch.h) - Batch rendering utilities
- [src/sdl/imageatlas.h](src/sdl/imageatlas.h) - Texture atlas management
- [src/sdl/gamecontroller.h](src/sdl/gamecontroller.h) - Game controller input
- [src/sdl/gpuutil.h](src/sdl/gpuutil.h) - Additional GPU utilities

### Dependencies (via vcpkg)
- SDL3 (with Vulkan support)
- SDL3_image (with PNG support)
- SDL3_ttf
- GLM (OpenGL Mathematics)
- spdlog (header-only)
- fmt
- FreeType
- GoogleTest (for tests)

## Code Style

### General Guidelines
- Write clear, maintainable, and well-documented code.
- Use tabs for indentation

### C++ code
- Use C++23 standard features (avoid modules).
- Follow the C++ Core Guidelines and modern best practices.
- Use smart pointers and RAII; avoid raw pointers unless absolutely necessary.
- Use `const` and `constexpr` wherever possible.
- Prefer brace initialization `{}` for variables and objects, except for inline member initializers where `=` is preferred.
- For member variables:
  - Use inline initialization with `=`.
  - End names with a trailing underscore (`score_`).
- Prefer `enum class` over traditional enums.
- Use classic header guards (`#ifndef`, `#define`, `#endif`) with guard names like `APP_GAME_INPUT_H` for `app::game` namespace in file `input.h`.
- Prefer `std` library facilities over C-style code and avoid macros.
- Write clear, self-documenting code with meaningful names.
- For virtual functions, use one of `override`, `final`, or `virtual`.
- Use `nullptr` instead of `NULL` or `0`.
- Avoid prefixing class names with "I" (use `Player` instead of `IPlayer`). For concrete implementations, use `PlayerImpl` or a more descriptive name if possible.
- For nested namespaces, use the compact syntax `namespace a::b::c { … }`.
- Logging is done with `spdlog`, and log messages should be prefixed with the class name in square brackets, for example `spdlog::info("[GpuContext] GPU driver available: {}", driverName);`.

### Header Guards
- Use classic `#ifndef`/`#define`/`#endif` guards
- Format: `NAMESPACE_FILENAME_H` (e.g., `CPPSDL3_SDL_WINDOW_H`)

### Compiler Warnings
- MSVC: `/W3 /WX /permissive-`
- GCC/Clang: `-Wall -Wextra -Wnon-virtual-dtor -pedantic -Wcast-align -Woverloaded-virtual -Wno-unused-parameter`

## Development Workflow

### Adding New GPU Resources
1. Add type alias and factory function in [src/sdl/gpu.h](src/sdl/gpu.h)
2. Use `GpuResourceDeleter` template with appropriate SDL release function
3. Ensure the deleter properly tracks the `SDL_GPUDevice`

### Compiling Shaders
```bash
python compile_shaders.py shader.hlsl EntryPoint target_profile -n namespace -g GUARD_PREFIX
# Example:
python compile_shaders.py shader.vs.hlsl VSMain vs_6_0 -n sdl -g CPPSDL3_SDL
```

### Visual Studio Integration
- `.natvis` and `.natstepfilter` files provide debugging visualizers
- Project uses folder organization via `USE_FOLDERS` property
- External dependencies grouped in `ExternalDependencies` folder
- PDB files installed for Debug and RelWithDebInfo configurations
