#pragma once
#include "Board.hpp"

class Player
{
public:
    Board myBoard;
    Board opponentBoard;

	//Variables for win condition
    int shipsToPlace;
    int hitsScored;

    Player(); 

    void setupBoard();
    void drawGameScreen();

    void recordHit(); 
};