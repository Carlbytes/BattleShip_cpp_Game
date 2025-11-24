#include "Player.hpp"
#include <iostream>
#include <vector>

// --- Cross-Platform Input Handling ---
#ifdef _WIN32
#include <conio.h>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_R 114

int getKeyPress() {
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        return _getch(); // Extended code
    }
    return ch;
}

void clearScreen() {
    system("cls");
}

#else
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_RIGHT 67
#define KEY_LEFT 68
#define KEY_ENTER 10
#define KEY_R 114

// Linux implementation of getch()
int getKeyPress() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();
    if (ch == 27) { // Escape sequence for arrows
        getchar(); // Skip [
        ch = getchar(); // Actual code
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void clearScreen() {
    system("clear");
}
#endif
// -------------------------------------

Player::Player()
{
    // Total hits required to win: 1x4 + 2x3 + 2x3 + 2x2 + 2x2 = 14
    totalShipHealth = 14;
    hitsScored = 0;
}

void Player::recordHit()
{
    hitsScored++;
}

void Player::setupBoard()
{
    // Define the fleet: size and color ID
    // 1x4, 1x3, 1x3, 1x2, 1x2
    struct ShipReq { int size; int color; };
    std::vector<ShipReq> fleet = {
        {4, 1}, // Cyan
        {3, 2}, // Yellow
        {3, 3}, // Magenta
        {2, 4}, // Green
        {2, 5}  // Blue
    };

    int cursorX = 0;
    int cursorY = 0;
    bool horizontal = true;

    for (size_t i = 0; i < fleet.size(); ++i)
    {
        bool placed = false;
        while (!placed)
        {
            bool valid = myBoard.isValidPlacement(cursorX, cursorY, fleet[i].size, horizontal);
            myBoard.displayWithCursor(true, cursorX, cursorY, fleet[i].size, horizontal, valid);

            std::cout << "Placing Ship " << (i+1) << "/" << fleet.size()
                      << " (Size: " << fleet[i].size << ")\n";

            int key = getKeyPress();

            switch(key) {
                case KEY_UP:    if(cursorY > 0) cursorY--; break;
                case KEY_DOWN:  if(cursorY < 9) cursorY++; break;
                case KEY_LEFT:  if(cursorX > 0) cursorX--; break;
                case KEY_RIGHT: if(cursorX < 9) cursorX++; break;
                case KEY_R: horizontal = !horizontal; break;
                case KEY_ENTER:
                    if (valid) {
                        myBoard.placeShip(cursorX, cursorY, fleet[i].size, horizontal, fleet[i].color);
                        placed = true;
                    }
                    break;
            }
        }
    }
    clearScreen();
    std::cout << "All ships placed!\n";
}

void Player::drawGameScreen()
{
    clearScreen();
    std::cout << "--- YOUR BOARD (Your Ships) ---\n";
    myBoard.display(true); // Show my ships with colors
    std::cout << "\n--- OPPONENT'S BOARD (Your Shots) ---\n";
    opponentBoard.display(false); // Hide their ships
    std::cout << "-----------------------------------\n";
}