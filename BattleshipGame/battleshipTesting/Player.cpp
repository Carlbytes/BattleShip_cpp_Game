#include "Player.hpp"
#include <iostream>

//Player class constructor
Player::Player()

{
	// Initialize variables for win condition
    shipsToPlace = 5; 
    hitsScored = 0; 
}

//Records a hit by incrementing hitsScored
void Player::recordHit()
{
	// Increment hits scored
    hitsScored++;
}

//Sets up the player's board by placing ships
void Player::setupBoard()
{
	// Prompt player to place ships
    std::cout << "Place your " << shipsToPlace << " ships (1-tile each for simplicity).\n";
	// Loop to place each ship
	for (int i = 0; i < shipsToPlace; ++i)
    {
		// Get coordinates for ship placement
        int x, y;
        std::cout << "Ship " << (i + 1) << " - Enter X coordinate (0-9): ";
        std::cin >> x;
        std::cout << "Ship " << (i + 1) << " - Enter Y coordinate (0-9): ";
        std::cin >> y;
		// Place the ship on the board
        myBoard.placeShip(x, y);
		// Redraw the game screen to show updated board
        drawGameScreen();
    }
    std::cout << "Ship placement complete.\n";
}

//Draws the game screen showing both boards
void Player::drawGameScreen()
{
    // "Clear" the console screen
    system("cls"); // Use "clear" on Linux/macOS

    std::cout << "--- YOUR BOARD (Your Ships) ---\n";
    myBoard.display(true); // Show my ships
    std::cout << "\n--- OPPONENT'S BOARD (Your Shots) ---\n";
    opponentBoard.display(false); // Hide their ships
    std::cout << "-----------------------------------\n";
}