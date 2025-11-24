cmake_minimum_required(VERSION 3.17)

# Project definition
project(BattleshipGame VERSION 1.0 LANGUAGES CXX)

# SFML 3 requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find SFML 3 ---
# We request all modules to ensure full functionality
# even though our current code primarily uses Network and System.
find_package(SFML 3 COMPONENTS Network System Graphics Window Audio REQUIRED)

# --- Server Executable ---
add_executable(BattleShipServer
        battleshipTesting/server.cpp
        battleshipTesting/Board.cpp
        battleshipTesting/Player.cpp
        # Headers are added for IDE visibility
        battleshipTesting/Board.hpp
        battleshipTesting/Player.hpp
)

# Include directory so #include "Player.hpp" works inside source files
target_include_directories(BattleShipServer PRIVATE battleshipTesting)

# Link SFML libraries
target_link_libraries(BattleShipServer PRIVATE
        SFML::Network
        SFML::System
        SFML::Graphics
        SFML::Window
        SFML::Audio
)

# --- Client Executable ---
add_executable(BattleShipClient
        BattleShipClient/client.cpp
        # Reusing the shared logic files from the sibling directory
        battleshipTesting/Board.cpp
        battleshipTesting/Player.cpp
        battleshipTesting/Board.hpp
        battleshipTesting/Player.hpp
)

# Include directory is critical here so client.cpp can find "Player.hpp" in the other folder
target_include_directories(BattleShipClient PRIVATE battleshipTesting)

# Link SFML libraries
target_link_libraries(BattleShipClient PRIVATE
        SFML::Network
        SFML::System
        SFML::Graphics
        SFML::Window
        SFML::Audio
)