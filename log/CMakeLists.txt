add_library(logger STATIC logger.cpp)

target_include_directories(logger PUBLIC ${Spdlog_INCLUDE_DIRS})
target_link_directories(logger PUBLIC ${Spdlog_LIBRARIES})
target_link_libraries(logger spdlogd)

if (MSVC)
    target_compile_options(logger PUBLIC "/utf-8")
endif()