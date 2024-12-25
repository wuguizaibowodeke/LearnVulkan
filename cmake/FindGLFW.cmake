list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/GLFW")
find_package(glfw3 REQUIRED)
if (glfw3_FOUND)
    #message(STATUS "cmake cmake GLFW found : ${GLFW_INCLUDE_DIRS}")
    #message(STATUS "cmake cmake GLFW found : ${GLFW_LIBRARIES}")
    #glfw3的-config.cmake貌似没有指定头文件目录，所以这里手动指定
    set(GLFW_INCLUDE_DIRS "C:/Program Files (x86)/GLFW/include")
    set(GLFW_LIBRARIES "C:/Program Files (x86)/GLFW/lib")
    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories(${GLFW_LIBRARIES})
endif ()