#include "Player.hpp"
#include <iostream>

// --- Cross-Platform Input Handling ---
#ifdef _WIN32
#include <conio.h>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_SPACE 32
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

// FIX: IDs changed to 1000+ to avoid conflict with ASCII 'A', 'B', etc.
#define KEY_UP 1001
#define KEY_DOWN 1002
#define KEY_RIGHT 1003
#define KEY_LEFT 1004
#define KEY_ENTER 10
#define KEY_SPACE 32
#define KEY_R 114

// Linux implementation of getch()
static int getKeyPress() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();
    if (ch == 27) { // Escape sequence
        getchar(); // Skip '['
        int code = getchar(); // Actual code
        switch(code) {
            case 'A': ch = KEY_UP; break;
            case 'B': ch = KEY_DOWN; break;
            case 'C': ch = KEY_RIGHT; break;
            case 'D': ch = KEY_LEFT; break;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
static void clearScreen() { system("clear"); }
#endif
// -------------------------------------

Player::Player()
{
    totalShipHealth = 14;
    hitsScored = 0;

    // OOP: Instantiating specific classes into a polymorphic container
    // This meets the requirement for Inheritance and Polymorphism
    fleet.push_back(std::make_unique<Battleship>());
    fleet.push_back(std::make_unique<Cruiser>());
    fleet.push_back(std::make_unique<Submarine>());
    fleet.push_back(std::make_unique<Destroyer>());
    fleet.push_back(std::make_unique<PatrolBoat>());
}

void Player::recordHit()
{
    hitsScored++;
}

void Player::setupBoard()
{
    int cursorX = 0;
    int cursorY = 0;
    bool horizontal = true;

    // OOP: Looping through objects using the Base Class pointer
    // This demonstrates polymorphism (getName() behaves differently for each ship)
    for (size_t i = 0; i < fleet.size(); ++i)
    {
        Ship* currentShip = fleet[i].get();

        bool placed = false;
        while (!placed)
        {
            // Use getters from the class (Encapsulation)
            bool valid = myBoard.isValidPlacement(cursorX, cursorY, currentShip->getSize(), horizontal);
            myBoard.displayWithCursor(true, cursorX, cursorY, currentShip->getSize(), horizontal, valid);

            std::cout << "Placing " << currentShip->getName() << " (" << (i + 1) << "/" << fleet.size() << ")\n";
            std::cout << "Size: " << currentShip->getSize() << "\n";
            std::cout << "Controls: WASD/Arrows to Move, R to Rotate, ENTER/SPACE to Place\n";

            int key = getKeyPress();

            switch (key) {
                // Up
            case KEY_UP:
            case 'w': case 'W':
                if (cursorY > 0) cursorY--;
                break;

                // Down
            case KEY_DOWN:
            case 's': case 'S':
                if (cursorY < 9) cursorY++;
                break;

                // Left
            case KEY_LEFT:
            case 'a': case 'A':
                if (cursorX > 0) cursorX--;
                break;

                // Right
            case KEY_RIGHT:
            case 'd': case 'D':
                if (cursorX < 9) cursorX++;
                break;

                // Rotate
            case 'r': case 'R':
                horizontal = !horizontal;
                break;

                // Place Ship
            case KEY_ENTER:
            case ' ': // Spacebar
                if (valid) {
                    // Pass the ID from the object to the board
                    myBoard.placeShip(cursorX, cursorY, currentShip->getSize(), horizontal, currentShip->getColorId());
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
    // clearScreen(); // Uncomment if you want to clear the history
    std::cout << "--- YOUR BOARD (Your Ships) ---\n";
    myBoard.display(true);
    std::cout << "\n--- OPPONENT'S BOARD (Your Shots) ---\n";
    opponentBoard.display(false);
    std::cout << "-----------------------------------\n";
}