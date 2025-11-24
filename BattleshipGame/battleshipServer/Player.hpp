#pragma once
#include "Board.hpp"

class Player
{
public:
    Board myBoard;
    Board opponentBoard;

    // Variables for win condition
    int totalShipHealth; // Total number of SHIP tiles
    int hitsScored;

    Player(); 

    void setupBoard();
    void drawGameScreen();

    void recordHit(); 
};