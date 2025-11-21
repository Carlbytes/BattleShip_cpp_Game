#pragma once
#include <iostream>

// The state of a single tile on the board
enum class TileState
{
    EMPTY,
    SHIP,
    HIT,
    MISS
};

class Board
{
public:
    // Constructor: Initializes the board to be empty
    Board();

    // Prints the board to the console
    void display(bool showShips);

    // Checks a shot at (x, y) and updates the grid
    TileState checkShot(int x, int y);

    // Places a ship (simplified to one tile for this example)
    void placeShip(int x, int y);

    // Checks if all ships have been sunk
    bool isGameOver();

    // Public functions to update the opponent's board (our tracker)
    void markHit(int x, int y);
    void markMiss(int x, int y);

private:
    TileState grid[10][10];
};
