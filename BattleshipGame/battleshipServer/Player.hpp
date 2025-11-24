#pragma once
#include "Board.hpp"
#include "Ship.hpp"  // <--- NEW INCLUDE
#include <vector>
#include <memory>    // <--- For std::unique_ptr

class Player
{
public:
    Board myBoard;
    Board opponentBoard;

    // We use a vector of pointers to the base class 'Ship'
    // This allows us to store Battleships and Cruisers in the same list (Polymorphism)
    std::vector<std::unique_ptr<Ship>> fleet;

    int totalShipHealth;
    int hitsScored;

    Player();

    void setupBoard();
    void drawGameScreen();
    void recordHit();
};