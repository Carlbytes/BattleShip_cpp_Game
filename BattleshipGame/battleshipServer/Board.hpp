#pragma once
#include <iostream>
#include <vector>

// The state of a single tile on the board
enum class TileState
{
    EMPTY,
    SHIP,
    HIT,
    MISS
};

// Struct to hold state and color info
struct Tile
{
    TileState state = TileState::EMPTY;
    int colorCode = 0; // 0: Default/Water, 1-5: Ship specific IDs
};

class Board
{
public:
    Board();

    // Standard display for game loop
    void display(bool showShips);

    // Interactive display for ship placement
    void displayWithCursor(bool showShips, int cursorX, int cursorY, int shipSize, bool horizontal, bool valid);

    // Checks a shot at (x, y) and updates the grid
    TileState checkShot(int x, int y);

    // Checks if a specific ship placement is valid
    bool isValidPlacement(int x, int y, int size, bool horizontal);

    // Places a ship of specific size and color
    void placeShip(int x, int y, int size, bool horizontal, int colorId);

    // Checks if all ships have been sunk
    bool isGameOver();

    // Check if a ship with a specific color ID is completely sunk (For Defender)
    bool isShipSunk(int colorId);

    // Getters
    int getTileColor(int x, int y);
    TileState getTileState(int x, int y);

    // Public functions to update the opponent's board (For Attacker)
    void markHit(int x, int y, int colorId);
    void markMiss(int x, int y);

    // Helper to get ship size based on ID (shared knowledge)
    static int getShipSize(int colorId);
    // Count how many hits we have recorded for a specific color
    int countHits(int colorId);

private:
    Tile grid[10][10];
    std::string getColorString(int colorCode);
};