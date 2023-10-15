cmake_minimum_required(VERSION 3.10)
project(SnakeGameProject)

# add_compile_definitions(DEBUG)

find_package(X11 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 17)

add_executable(SnakeGame
    main.cpp
    src/game_display.cpp
    src/snake_game.cpp
    src/logger.cpp
    )

target_link_libraries(SnakeGame
    ${X11_LIBRARIES}
    )