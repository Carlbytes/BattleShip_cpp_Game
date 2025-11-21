#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <optional>
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

// Tries to connect to the server at the given IP and port
bool tryConnect(sf::TcpSocket& socket, const std::string& ipString, unsigned short port)
{
	// Attempt to resolve the IP address
    std::cout << "Trying to connect to " << ipString << "..." << std::endl;
	//This line will use std::optional to handle potential resolution failure
    std::optional<sf::IpAddress> serverIpOptional = sf::IpAddress::resolve(ipString);
	// Check if resolution was successful
    if (!serverIpOptional)
    {
		//error handling for failed resolution
        std::cerr << "Error: Could not resolve IP address '" << ipString << "'" << std::endl;
        return false;
    }
	// Attempt to connect to the server
    sf::IpAddress serverIp = serverIpOptional.value();
	// Try to connect
    if (socket.connect(serverIp, port) == sf::Socket::Status::Done)
    {
        return true; // Success!
    }
    else
    {
        std::cerr << "Connection failed." << std::endl;
        return false; // Failed
    }
}

// Entry point of the client application
int main()
{
	// Set up the client socket and connection parameters
    sf::TcpSocket socket;
    unsigned short port = 54000;
    std::string ipAddressString;
    const std::string configFilename = "ip_config.txt";


	// --- CONNECTION LOGIC ---
    std::ifstream configFileIn(configFilename);
    if (configFileIn.is_open()) { std::getline(configFileIn, ipAddressString); configFileIn.close(); }
    else { ipAddressString = "127.0.0.1"; }
    bool isConnected = tryConnect(socket, ipAddressString, port);
	// If initial connection fails, prompt for new IP
    if (!isConnected)
    {
		//If connection fails, prompt user for new IP
        std::cout << "\nPlease enter a new server IP address: ";
		//saves the new IP address
        std::cin >> ipAddressString;
		//Try to connect again with the new IP
        isConnected = tryConnect(socket, ipAddressString, port);
		//If connection is successful, save the new IP to config file
        if (isConnected) { std::ofstream configFileOut(configFilename); if (configFileOut.is_open()) { configFileOut << ipAddressString; configFileOut.close(); std::cout << "New IP saved.\n"; } }
    }
	// Handle failed connection
    if (!isConnected) { std::cerr << "Could not connect. Exiting.\n"; return 1; }

    std::cout << "Connected to server! Setting up game...\n";

	// ~~Game Setup~~
	// ----------------------
	Player myPlayer;
    myPlayer.setupBoard();
    // ----------------------

    sf::Packet packet;
    int messageType;
    std::string chatMessage;
    int x_coord;
    int y_coord;
    std::string responseString;

    std::cin.ignore(1000, '\n'); // Clear buffer
    GameState currentState = GameState::MY_TURN; // Client starts

    while (true)
    {
        if (currentState == GameState::MY_TURN)
        {
            myPlayer.drawGameScreen();

            //Win condition check
        	if (myPlayer.hitsScored >= myPlayer.shipsToPlace)
            {
                std::cout << "\n*** ALL ENEMY SHIPS SUNK! YOU WIN! ***\n";
                break;
            }

			// Prompt for action
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
                std::getline(std::cin, chatMessage);
                packet << messageType << chatMessage;
				// Send the packet to the server
                if (socket.send(packet) != sf::Socket::Status::Done) break;
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
				// Send the packet to the server
                if (socket.send(packet) != sf::Socket::Status::Done) break;

				//  Await and process server's response
                std::cout << "Waiting for server's reply..." << std::endl;
                if (socket.receive(packet) != sf::Socket::Status::Done) break;

                int responseType = 0;
				// Extract response type and string
                if (packet >> responseType >> responseString && responseType == MessageType::GAME_RESULT)
                {
					// Display server's response
                    std::cout << "Server said: " << responseString << std::endl;

					// --- UPDATE OPPONENT'S BOARD ---
                    if (responseString == "You Hit!")
                    {
                        myPlayer.opponentBoard.markHit(x_coord, y_coord);
                        myPlayer.recordHit(); // CHANGED: Record the successful hit
                    }
					// If it was a miss
                    else
                    {
                        myPlayer.opponentBoard.markMiss(x_coord, y_coord);
                    }
                }
                currentState = GameState::OPPONENTS_TURN;
            }
        }
		// Handle opponent's turn
        else // currentState == GameState::OPPONENTS_TURN
        {
            myPlayer.drawGameScreen();

            // Check for a loss
            if (myPlayer.myBoard.isGameOver())
            {
                std::cout << "\n*** ALL YOUR SHIPS ARE SUNK! YOU LOSE! ***\n";
                break;
            }

            std::cout << "\nWaiting for opponent's move..." << std::endl;
            if (socket.receive(packet) != sf::Socket::Status::Done)
            {
                std::cout << "Server disconnected." << std::endl;
                break;
            }
			// Extract message type
            if (!(packet >> messageType))
            {
                std::cerr << "Error: Malformed packet." << std::endl;
                continue;
            }

			// Process based on message type
            if (messageType == MessageType::CHAT)
            {
                if (packet >> chatMessage)
                {
                    std::cout << "Opponent chat: " << chatMessage << std::endl;

					// Pause to let user read the message
                	std::cout << "(Press Enter to continue...)";
                    std::string dummy;
                    std::getline(std::cin, dummy); // This waits for the user to press Enter
                }
            }
            
            else if (messageType == MessageType::FIRE_SHOT)
            {
                if (packet >> x_coord >> y_coord)
                {
                    std::cout << "Opponent fired at (" << x_coord << ", " << y_coord << ")" << std::endl;
					// --- CHECK SHOT RESULT ---
                    TileState result = myPlayer.myBoard.checkShot(x_coord, y_coord);
                    if (result == TileState::HIT)
                    {
                        responseString = "You Hit!";
                    }
                    else
                    {
                        responseString = "You Missed!";
                    }
                    // -----------------------------------------

                    int responseType = MessageType::GAME_RESULT;
                    packet.clear();
                    packet << responseType << responseString;
                    if (socket.send(packet) != sf::Socket::Status::Done) break;
                }
                currentState = GameState::MY_TURN;
            }
        }
    }
    system("pause"); // Wait before closing
    return 0;
}