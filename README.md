# Tech Stack
- C++
- Vulkan
- GLFW
- GLM
- CMake
- MinGW64

# Install Links
- Vulkan: https://vulkan.lunarg.com/
- GLFW Pre-Compilied Binaries: https://www.glfw.org/download.html
- GLM: https://github.com/g-truc/glm/releases
- CMake: https://cmake.org/
- MingW-W64-builds: https://www.mingw-w64.org/downloads/


# Setting up project
- Make sure to install any thing from the list above if not installed
- Add Vulkan and MinGW64 to your path
- Then update envExample.cmake with the correct path of your installs and rename it to .env.cmake
- Next run build.bat or buildRun.bat to generate the build
- If using vscode make sure to add the Vulkan, GLFW, and GLM include paths to the c_cpp_properties.json file and the mingw c++ compiler 
- (CTRL+Shift+P then select C/C++: Edit Configurations (UI) )

# Tutorial to Follow
- https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=1