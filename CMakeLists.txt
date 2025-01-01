cmake_minimum_required(VERSION 3.15)
project(toy2D)

set(CMAKE_CXX_STANDARD 17)

include(cmake/FindVulkan.cmake)
include(cmake/FindGLFW.cmake)
include(cmake/FindSpdlog.cmake)

if (NOT Vulkan_FOUND)
    message(STATUS "Vulkan Not found.")
endif ()
if (NOT glfw3_FOUND)
    message(STATUS "GLFW Not found.")
endif ()
if (NOT spdlog_FOUND)
    message(STATUS "spdlog Not found.")
endif ()

add_subdirectory(log)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/log)

aux_source_directory(src SRC)
add_library(toy2d STATIC ${SRC})
target_include_directories(toy2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_include_directories(toy2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include)
target_link_libraries(toy2d PUBLIC Vulkan::Vulkan glfw3 logger)

add_subdirectory(sandbox)

