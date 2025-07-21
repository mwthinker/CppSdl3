# sdl::CppSdl3 [![CI build](https://github.com/mwthinker/CppSdl3/actions/workflows/ci.yml/badge.svg)](https://github.com/mwthinker/CppSdl3/actions/workflows/ci.yml) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
A C++ wrapper around SDL3 (Simple Direct Media Layer) using SDL_gpu together with ImGui (Dear ImGui).

## Developer environment
This project is developed using [vcpkg](https://github.com/microsoft/vcpkg.git) and [CMake](https://cmake.org/).

Aassuming unix host otherwise change to "windows"
```bash
git clone https://github.com/mwthinker/CppSdl3; cd CppSdl3
mkdir build
cd build
cmake --preset=unix -DCppSdl3_Example=1 -DCppSdl3_Test=1 ..
make
```

## Usage
When using CMake, in CMakeLists.txt

```cmake
include(FetchContent)
# Load external github projects
FetchContent_Declare(CppSdl3
    GIT_REPOSITORY
        https://github.com/mwthinker/CppSdl3.git
    OVERRIDE_FIND_PACKAGE
)

# ... some code
find_package(CppSdl3 REQUIRED)

# ... some code
target_link_libraries(<YourProject>
    PRIVATE
        CppSdl3
)
```

In code main.cpp

```cpp
#include <sdl/window.h>

class ExampleWindow : public sdl::Window {
public:
    ExampleWindow() = default;

private:
    void renderImGui(const sdl::DeltaTime& deltaTime) override {
        ImGui::MainWindow("ExampleWindow", [this](){
            if (ImGui::Button("Quit")) {
                quit();
            }
        });
    }

    void processEvent(const SDL_Event& windowEvent) override {
        switch (windowEvent.type) {
            case SDL_WINDOWEVENT:
                switch (windowEvent.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        quit();
                        break;
                }
                break;
            case SDL_KEYDOWN:
                switch (windowEvent.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit();
                        break;
                }
                break;
            case SDL_QUIT:
                quit();
                break;
        }
    }
};

int main() {
    ExampleWindow exampleWindow;
    exampleWindow.startLoop();
    return 0;
}

```

## Open source
The code is licensed under the MIT License (see LICENSE.txt).
