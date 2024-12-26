LearnVulkan
===
LearnVulkan是一个简单的2d渲染器，目的是为了了解vulkan的相关API.

编译
-----
工程使用CMake，需要预先安装好VulkanSDK，glfw3，spdlog
将上述三个三方库安装后之后，将cmake/FindVulkan.cmake中的路径替换为自己的安装路径
```cmake
set(ENV{VULKAN_SDK} "<your dir to VulkanSDK>")
```
将cmake/FindGLFW.cmake中的路径替换为自己的安装路径
```cmake
list(APPEND CMAKE_PREFIX_PATH "<your dir to GLFW>")
...
set(GLFW_INCLUDE_DIRS "<your dir to GLFW include>")
set(GLFW_LIBRARIES "<your dir to GLFW lib>")
```
将cmake/FindSpdlog.cmake中的路径替换为自己的安装路径
```cmake
list(APPEND CMAKE_PREFIX_PATH "<your dir to spdlog>")
...
set(Spdlog_INCLUDE_DIRS "<your dir to spdlog include>")
set(Spdlog_LIBRARIES "<your dir to spdlog lib>")
```
之后正常编译运行sandbox.exe即可
