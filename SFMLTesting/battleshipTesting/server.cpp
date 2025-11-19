#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "Player.hpp"

//Gives the options for the type of message being sent
enum MessageType
{
    CHAT = 1,
    FIRE_SHOT = 2,
    GAME_RESULT = 3
};

// Represents the current state of the game
enum GameState
{
    MY_TURN,
    OPPONENTS_TURN
};
// Entry point of the server application
int main()
{
	// Set up the server to listen for incoming connections
    sf::TcpListener listener;
	// Listen on port 54000
    unsigned short port = 54000;
	// Start listening for connections
    if (listener.listen(port) != sf::Socket::Status::Done)
    {
		// Error handling
        std::cerr << "Error: Could not listen on port " << port << std::endl;
        return 1;
    }
	// Inform that the server is waiting for a client
    std::cout << "Server is listening on port " << port << ", waiting for a client..." << std::endl;

	// Accept a new client connection
    sf::TcpSocket client;
	// Wait for a client to connect
    if (listener.accept(client) != sf::Socket::Status::Done)
    {
		// Error handling
        std::cerr << "Error: Could not accept client connection" << std::endl;
        return 1;
    }
	// Inform that a client has connected
    std::cout << "Client connected from: " << client.getRemoteAddress().value().toString() << std::endl;
	// Set up the game
    std::cout << "Setting up game...\n";

	// ~~Game Setup~~
    Player myPlayer;
    myPlayer.setupBoard();

	// Main game loop variables
    sf::Packet packet;
    int messageType;
    std::string chatMessage;
    int x_coord;
    int y_coord;
    std::string responseString;

	// Clear input buffer
    std::cin.ignore(1000, '\n'); // Clear buffer

	// Start with opponent's turn since server goes second
    GameState currentState = GameState::OPPONENTS_TURN; // Server goes second

    while (true)
    {
		// MY TURN
        if (currentState == GameState::MY_TURN)
        {
			// Display the current game screen
            myPlayer.drawGameScreen();

			// Check for a win
            if (myPlayer.hitsScored >= myPlayer.shipsToPlace)
            {
                std::cout << "\n*** ALL ENEMY SHIPS SUNK! YOU WIN! ***\n";
                break;
            }

			// Prompt for action: chat or fire
            std::cout << "\nMY TURN: (c) to chat or (f) to fire: ";
            char action;
            std::cin >> action;
            std::cin.ignore(1000, '\n');
            packet.clear();

			// Handle chat action
            if (action == 'c' || action == 'C')
            {
                messageType = MessageType::CHAT;
                std::cout << "Enter your message: ";
				// Read the entire line for the chat message
                std::getline(std::cin, chatMessage);
				// Package and send the chat message
                packet << messageType << chatMessage;
				// Send the packet to the client
                if (client.send(packet) != sf::Socket::Status::Done) break;
            }
			// Handle fire action
            else if (action == 'f' || action == 'F')
            {
                messageType = MessageType::FIRE_SHOT;
                std::cout << "Enter X coordinate: ";
                std::cin >> x_coord;
                std::cout << "Enter Y coordinate: ";
                std::cin >> y_coord;
                std::cin.ignore(1000, '\n');

				// Package and send the fire shot message
                packet << messageType << x_coord << y_coord;
				// Send the packet to the client
                if (client.send(packet) != sf::Socket::Status::Done) break;

				// Wait for the client's reply
                std::cout << "Waiting for client's reply..." << std::endl;
				// Clear the packet for receiving
                if (client.receive(packet) != sf::Socket::Status::Done) break;

				// Process the client's response
                int responseType = 0;
				// Extract response type and string
                if (packet >> responseType >> responseString && responseType == MessageType::GAME_RESULT)
                {
                    std::cout << "Client said: " << responseString << std::endl;
					// Update opponent's board based on the result
                    if (responseString == "You Hit!")
                    {
						// Mark the hit on opponent's board
                        myPlayer.opponentBoard.markHit(x_coord, y_coord);
                        myPlayer.recordHit(); // CHANGED: Record the successful hit
                    }
                    else
                    {
						// Mark the miss on opponent's board
                        myPlayer.opponentBoard.markMiss(x_coord, y_coord);
                    }
                }
                currentState = GameState::OPPONENTS_TURN;
            }
        }
        else // currentState == GameState::OPPONENTS_TURN
        {
            myPlayer.drawGameScreen();

            // Check for a loss
            if (myPlayer.myBoard.isGameOver())
            {
                std::cout << "\n*** ALL YOUR SHIPS ARE SUNK! YOU LOSE! ***\n";
                break;
            }

			// Wait for opponent's move
            std::cout << "\nWaiting for opponent's move..." << std::endl;
			// Clear the packet for receiving
        	if (client.receive(packet) != sf::Socket::Status::Done)
            {
                
                std::cout << "Client disconnected." << std::endl;
                break;
            }

			// Extract message type
            if (!(packet >> messageType))
            {
				//Error handling for malformed packet
                std::cerr << "Error: Malformed packet." << std::endl;
                continue;
            }

			// Process the opponent's message
            if (messageType == MessageType::CHAT)
            {
				// Extract chat message
                if (packet >> chatMessage)
                {
                    std::cout << "Opponent chat: " << chatMessage << std::endl;

					// Pause to let user read the message
                	std::cout << "(Press Enter to continue...)";
                    std::string dummy;
                    std::getline(std::cin, dummy); // This waits for the user to press Enter
                }
            }
			//MessageType::FIRE_SHOT
            else if (messageType == MessageType::FIRE_SHOT)
            {
				// Extract coordinates
                if (packet >> x_coord >> y_coord)
                {
                    std::cout << "Opponent fired at (" << x_coord << ", " << y_coord << ")" << std::endl;

                    // --- CHECK OUR BOARD FOR A REAL RESULT ---
                    TileState result = myPlayer.myBoard.checkShot(x_coord, y_coord);
					//Checks if the shot was a hit and updates hitsScored
                    if (result == TileState::HIT)
                    {
                        responseString = "You Hit!";
                    }
					//If the shot was a miss
                    else
                    {
                        responseString = "You Missed!";
                    }
                    // -----------------------------------------

					// Send back the result to the opponent
                    int responseType = MessageType::GAME_RESULT;
					// Clear and package the response
                	packet.clear();
                    packet << responseType << responseString;
                    if (client.send(packet) != sf::Socket::Status::Done) break;
                }
                currentState = GameState::MY_TURN;
            }
        }
    }
    system("pause"); // Wait before closing
    return 0;
}