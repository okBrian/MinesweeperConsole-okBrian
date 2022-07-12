cmake_minimum_required(VERSION 2.20)

set(CMAKE_CXX_STANDARD 20)

Project(Minesweeper-OkBrian)

add_executable(${PROJECT_NAME} src/main.cpp src/MinesweeperGame.cpp src/MinesweeperGame.hpp)  

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)