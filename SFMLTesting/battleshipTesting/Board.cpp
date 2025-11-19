#include "Board.hpp"

// Board class constructor
Board::Board()
{
    // Initialize all tiles to EMPTY
    for (int x = 0; x < 10; ++x)
    {
		// Initialize each row
        for (int y = 0; y < 10; ++y)
        {
			// Set tile to EMPTY
            grid[x][y] = TileState::EMPTY;
        }
    }
}

// Displays the board to the console
void Board::display(bool showShips)
{
    // Print column headers
    std::cout << "  0 1 2 3 4 5 6 7 8 9\n";
    for (int y = 0; y < 10; ++y)
    {
        // Print row header
        std::cout << y << " ";
		// Print each tile in the row
        for (int x = 0; x < 10; ++x)
        {
			// Determine what to display based on tile state
            switch (grid[x][y])
            {
				// Tile is empty
            case TileState::EMPTY:
                std::cout << "~ ";
                break;
				// Tile has a ship
            case TileState::SHIP:
                // Only show 'S' if it's our own board
                if (showShips) std::cout << "S ";
                else std::cout << "~ ";
                break;
				// Tile has been hit
            case TileState::HIT:
                std::cout << "X ";
                break;
				// Tile was a miss
            case TileState::MISS:
                std::cout << "O ";
                break;
            }
        }
        std::cout << "\n";
    }
}

// Checks a shot at (x, y) and updates the grid
TileState Board::checkShot(int x, int y)
{
    if (grid[x][y] == TileState::SHIP)
    {
        grid[x][y] = TileState::HIT;
        return TileState::HIT;
    }
	// Shot missed
    else if (grid[x][y] == TileState::EMPTY)
    {
        grid[x][y] = TileState::MISS;
        return TileState::MISS;
    }
    // Already shot here
    return grid[x][y];
}

// Places a ship at (x, y)
void Board::placeShip(int x, int y)
{
    if (x >= 0 && x < 10 && y >= 0 && y < 10)
    {
        grid[x][y] = TileState::SHIP;
    }
}

// Checks if all ships have been sunk
bool Board::isGameOver()
{
    // If any 'SHIP' tile still exists, the game is not over
    for (int x = 0; x < 10; ++x)
    {
        for (int y = 0; y < 10; ++y)
        {
            if (grid[x][y] == TileState::SHIP)
            {
                return false;
            }
        }
    }
    // No 'SHIP' tiles were found
    return true;
}

// Marks a hit on the opponent's board at (x, y)
void Board::markHit(int x, int y)
{
	// Ensure coordinates are within bounds
    if (x >= 0 && x < 10 && y >= 0 && y < 10)
    {
        grid[x][y] = TileState::HIT;
    }
}

// Marks a miss on the opponent's board at (x, y)
void Board::markMiss(int x, int y)
{
	// Ensure coordinates are within bounds
    if (x >= 0 && x < 10 && y >= 0 && y < 10)
    {
        grid[x][y] = TileState::MISS;
    }
}