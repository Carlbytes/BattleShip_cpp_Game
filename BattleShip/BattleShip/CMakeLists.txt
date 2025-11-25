cmake_minimum_required(VERSION 3.10)
project(BattleshipGame)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CONFIG FOR SFML ---
# Disable building examples and docs to speed up the compile time
set(SFML_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_DOCS FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE FALSE CACHE BOOL "" FORCE)

# --- ADD SFML ---
# This looks inside the folder you just cloned and compiles it
add_subdirectory(External/SFML)

# --- ADD YOUR GAME ---
add_executable(battleship
    main.cpp
    GameManager.cpp
    Player.cpp
    Board.cpp
)

# --- LINK LIBRARIES ---
# Since we are building SFML from source, we use the specific target names
target_link_libraries(battleship PRIVATE SFML::Network SFML::System)
