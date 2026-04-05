cmake_minimum_required(VERSION 4.3.1)
project(cpp-gui-test)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE Vulkan::Vulkan glfw)