set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.296.0")
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    #message(STATUS "cmake cmake Vulkan found : ${Vulkan_INCLUDE_DIRS}")
    #message(STATUS "cmake cmake Vulkan found : ${Vulkan_LIBRARY}")
    include_directories(${Vulkan_INCLUDE_DIRS})
    link_directories(${Vulkan_LIBRARY})
endif ()