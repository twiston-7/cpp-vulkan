cmake_minimum_required(VERSION 4.2.2)
set(CMAKE_CXX_STANDARD 17)
project(cpp-gui-test)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(main 
    src/main.cpp
    src/setup.cpp
)

target_sources(main PRIVATE
    include/setup.h
    include/constants.h
    include/debug_callback.h
)

target_include_directories(main PRIVATE include)

target_link_libraries(main PRIVATE Vulkan::Vulkan glfw)