cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip8 main.cpp utils/non_copyable.h utils/Utils.h utils/Utils.cpp interpreter/VM.cpp interpreter/VM.h graphics/Window.cpp graphics/Window.h)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(chip8 ${SFML_LIBRARIES})
endif()