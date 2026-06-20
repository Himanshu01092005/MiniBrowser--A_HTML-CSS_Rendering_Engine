cmake_minimum_required(VERSION 3.16)
project(MiniBrowser)
set(CMAKE_CXX_STANDARD 17)

# SFML 3 requires capitalized component names
find_package(SFML COMPONENTS System Window Graphics CONFIG REQUIRED)

add_executable(MiniBrowser main.cpp)

# SFML 3 requires modern namespaced targets
target_link_libraries(MiniBrowser PRIVATE SFML::System SFML::Window SFML::Graphics)