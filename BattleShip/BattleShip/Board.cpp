#include "Board.hpp"
#include <iostream>

#ifdef _WIN32
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

// ANSI Color Codes
const std::string ANSI_RESET = "\033[0m";
const std::string ANSI_RED = "\033[31m";
const std::string ANSI_GREEN = "\033[32m";
const std::string ANSI_YELLOW = "\033[33m";
const std::string ANSI_BLUE = "\033[34m";
const std::string ANSI_MAGENTA = "\033[35m";
const std::string ANSI_CYAN = "\033[36m";
const std::string ANSI_WHITE = "\033[37m";
const std::string ANSI_BG_WHITE = "\033[47m";

// Constructor initializes the board with empty tiles
Board::Board()
{
	// Initialize all tiles to EMPTY state
    for (int x = 0; x < 10; ++x)
    {
		// Initialize each tile in the grid
        for (int y = 0; y < 10; ++y)
        {
			// Set each tile to EMPTY and color code to 0
            grid[x][y].state = TileState::EMPTY;
            grid[x][y].colorCode = 0;
        }
    }
}

// Maps color codes to ANSI color strings
std::string Board::getColorString(int colorCode)
{
	// Returns ANSI color code based on ship color ID
    switch (colorCode) {
    case 1: return ANSI_CYAN;
    case 2: return ANSI_YELLOW;
    case 3: return ANSI_MAGENTA;
    case 4: return ANSI_GREEN;
    case 5: return ANSI_BLUE;
    default: return ANSI_RESET;
    }
}

// Hardcoded definitions of fleet sizes based on Color ID
// 1: Cyan (4), 2: Yellow (3), 3: Magenta (3), 4: Green (2), 5: Blue (2)
int Board::getShipSize(int colorId)
{
    switch (colorId) {
    case 1: return 4;
    case 2: return 3;
    case 3: return 3;
    case 4: return 2;
    case 5: return 2;
    default: return 0;
    }
}

// Counts how many hits have been recorded for a specific ship color
int Board::countHits(int colorId)
{
    if (colorId == 0) return 0;
    int hits = 0;
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 10; ++y)
            if (grid[x][y].colorCode == colorId && grid[x][y].state == TileState::HIT)
                hits++;
    return hits;
}

// Displays the board in the console
void Board::display(bool showShips)
{
    std::cout << "   0 1 2 3 4 5 6 7 8 9\n";
    for (int y = 0; y < 10; ++y)
    {
        std::cout << y << "  ";
        for (int x = 0; x < 10; ++x)
        {
            Tile& t = grid[x][y];

            if (t.state == TileState::HIT)
            {
                bool sunk = false;

                if (showShips) {
                    // On MY board, I know if it's sunk by checking remaining parts
                    sunk = isShipSunk(t.colorCode);
                }
                else {
                    // On OPPONENT board, I only know hits.
                    // Check if hits match total size.
                    int totalSize = getShipSize(t.colorCode);
                    if (totalSize > 0 && countHits(t.colorCode) >= totalSize) {
                        sunk = true;
                    }
                }

				// Display differently if sunk
                if (sunk) std::cout << ANSI_RED << "O " << ANSI_RESET;
                else      std::cout << ANSI_RED << "X " << ANSI_RESET;
            }
			// Missed shot
            else if (t.state == TileState::MISS)
            {
                std::cout << ANSI_WHITE << "O " << ANSI_RESET;
            }
			// Ship tile
            else if (t.state == TileState::SHIP)
            {
                if (showShips)
                    std::cout << getColorString(t.colorCode) << "# " << ANSI_RESET;
                else
                    std::cout << "~ ";
            }
			//Empty tile
            else // EMPTY
            {
                std::cout << "~ ";
            }
        }
        std::cout << "\n";
    }
}

// Displays the board with a cursor for ship placement
void Board::displayWithCursor(bool showShips, int cursorX, int cursorY, int shipSize, bool horizontal, bool valid)
{
    system(CLEAR_CMD);
    std::cout << "   0 1 2 3 4 5 6 7 8 9\n";

    std::vector<std::pair<int, int>> ghostCells;
    for (int i = 0; i < shipSize; ++i) {
        if (horizontal) ghostCells.push_back({ cursorX + i, cursorY });
        else           ghostCells.push_back({ cursorX, cursorY + i });
    }

    for (int y = 0; y < 10; ++y)
    {
        std::cout << y << "  ";
        for (int x = 0; x < 10; ++x)
        {
            bool isCursor = false;
            for (auto& p : ghostCells) 
            {
                if (p.first == x && p.second == y)
                {
                    isCursor = true;
                    break;
                }
            }

            Tile& t = grid[x][y];

            if (isCursor)
            {
                std::string color = valid ? ANSI_GREEN : ANSI_RED;
                // If hovering over an existing hit/miss, show '?' to indicate "Are you sure?"
                // otherwise show 'X' or '#'
                char symbol = (t.state == TileState::SHIP || t.state == TileState::EMPTY) ? (t.state == TileState::SHIP ? 'X' : '#') : '?';
                std::cout << ANSI_BG_WHITE << color << symbol << " " << ANSI_RESET;
            }
            else
            {
				// This block shows the normal tile state

				//Checks  if the tile is a hit, miss, ship, or empty and displays accordingly
                if (t.state == TileState::HIT)
                {
                    bool sunk = false;
                    if (showShips) 
                    {
                        sunk = isShipSunk(t.colorCode);
                    }
                    else
                    {
                        int totalSize = getShipSize(t.colorCode);
                        if (totalSize > 0 && countHits(t.colorCode) >= totalSize) sunk = true;
                    }

                    if (sunk) std::cout << ANSI_RED << "O " << ANSI_RESET;
                    else      std::cout << ANSI_RED << "X " << ANSI_RESET;
                }
				//Sets the color and symbol for a missed shot
                else if (t.state == TileState::MISS)
                {
                    std::cout << ANSI_WHITE << "O " << ANSI_RESET;
                }
				//Sets the color and symbol for a ship tile
                else if (t.state == TileState::SHIP)
                {
					// Show ship only if allowed
                    if (showShips)
                        std::cout << getColorString(t.colorCode) << "# " << ANSI_RESET;
                    else
                        std::cout << "~ ";
                }
                else 
                {
                    std::cout << "~ ";
                }
                
            }
        }
		//turn to next line after each row
        std::cout << "\n";
    }
}

//checks if a shot hits a ship or misses and updates the tile state accordingly
TileState Board::checkShot(int x, int y)
{
    if (x < 0 || x >= 10 || y < 0 || y >= 10) return TileState::MISS;

    if (grid[x][y].state == TileState::SHIP)
    {
        grid[x][y].state = TileState::HIT;
        return TileState::HIT;
    }
    else if (grid[x][y].state == TileState::EMPTY)
    {
        grid[x][y].state = TileState::MISS;
        return TileState::MISS;
    }
    return grid[x][y].state;
}

//checks if a ship can be placed at the specified coordinates without overlapping or going out of bounds
bool Board::isValidPlacement(int x, int y, int size, bool horizontal)
{
    for (int i = 0; i < size; ++i)
    {
        int cx = horizontal ? x + i : x;
        int cy = horizontal ? y : y + i;

        if (cx < 0 || cx >= 10 || cy < 0 || cy >= 10) return false;
        if (grid[cx][cy].state != TileState::EMPTY) return false;
    }
    return true;
}

//places a ship on the board at the specified coordinates with the given size and orientation
void Board::placeShip(int x, int y, int size, bool horizontal, int colorId)
{
    if (!isValidPlacement(x, y, size, horizontal)) return;

    for (int i = 0; i < size; ++i)
    {
        int cx = horizontal ? x + i : x;
        int cy = horizontal ? y : y + i;
        grid[cx][cy].state = TileState::SHIP;
        grid[cx][cy].colorCode = colorId;
    }
}

//checks if all ships on the board have been sunk
bool Board::isGameOver()
{
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 10; ++y)
            if (grid[x][y].state == TileState::SHIP)
                return false;
    return true;
}

//checks if a specific ship identified by colorId has been completely sunk
bool Board::isShipSunk(int colorId)
{
    if (colorId == 0) return false;
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 10; ++y)
            if (grid[x][y].colorCode == colorId && grid[x][y].state == TileState::SHIP)
                return false;
    return true;
}

// Getters
int Board::getTileColor(int x, int y)
{
    if (x >= 0 && x < 10 && y >= 0 && y < 10)
        return grid[x][y].colorCode;
    return 0;
}

//returns the state of the tile at the specified coordinates
TileState Board::getTileState(int x, int y)
{
    if (x >= 0 && x < 10 && y >= 0 && y < 10)
        return grid[x][y].state;
    return TileState::MISS;
}

//marks a hit on the opponent's board at the specified coordinates and records the ship's color ID
void Board::markHit(int x, int y, int colorId)
{
    if (x >= 0 && x < 10 && y >= 0 && y < 10) 
    {
        grid[x][y].state = TileState::HIT;
        grid[x][y].colorCode = colorId; // Store the color so we can track the ship on opponent board
    }
}

//marks a miss on the opponent's board at the specified coordinates
void Board::markMiss(int x, int y)
{
    if (x >= 0 && x < 10 && y >= 0 && y < 10)
        grid[x][y].state = TileState::MISS;
}