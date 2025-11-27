#pragma once
#include "Board.hpp"    // <--- For Board class
#include "Ship.hpp"     // <--- For Ship base class and derived
#include <vector>   // <--- For std::vector
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