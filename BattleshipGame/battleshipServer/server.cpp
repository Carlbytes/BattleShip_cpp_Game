#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include "Player.hpp"

enum MessageType { CHAT = 1, FIRE_SHOT = 2, GAME_RESULT = 3, SETUP_COMPLETE = 4 };
enum GameState { MY_TURN, OPPONENTS_TURN };

void waitForKey() {
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

bool getCoordinates(int& x, int& y) {
    std::string line;
    std::getline(std::cin, line);
    for (char& c : line) {
        if (c == ',') c = ' ';
    }
    std::stringstream ss(line);
    if (ss >> x >> y) return true;
    return false;
}

std::string getShipName(int colorCode) {
    switch (colorCode) {
        case 1: return "Battleship (1x4)";
        case 2: return "Cruiser (1x3)";
        case 3: return "Submarine (1x3)";
        case 4: return "Destroyer (1x2)";
        case 5: return "Patrol Boat (1x2)";
        default: return "Ship";
    }
}

int main()
{
    sf::TcpListener listener;
    unsigned short port = 54000;

    if (listener.listen(port) != sf::Socket::Status::Done) return 1;
    std::cout << "Server listening on " << port << "...\n";

    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Status::Done) return 1;
    std::cout << "Client connected: " << client.getRemoteAddress().value().toString() << std::endl;

    sf::Packet packet;
    Player myPlayer;
    myPlayer.setupBoard();

    // --- Sync ---
    std::cout << "Waiting for opponent...\n";
    packet << (int)MessageType::SETUP_COMPLETE << "Ready";
    client.send(packet);

    bool opponentReady = false;
    while (!opponentReady) {
        packet.clear();
        if (client.receive(packet) == sf::Socket::Status::Done) {
            int type;
            if (packet >> type && type == MessageType::SETUP_COMPLETE) opponentReady = true;
        }
    }
    std::cout << "Game Start!\n";
    // ------------

    int messageType;
    std::string chatMessage;
    int x_coord, y_coord;
    std::string responseString;
    int hitColor = 0; // To store color received from packet

    GameState currentState = GameState::OPPONENTS_TURN;

    while (true)
    {
        if (currentState == GameState::MY_TURN)
        {
            myPlayer.drawGameScreen();

            if (myPlayer.hitsScored >= myPlayer.totalShipHealth) {
                std::cout << "\n*** YOU WIN! ***\n";
                break;
            }

            std::cout << "\nMY TURN: (c) to chat or (f) to fire: ";
            char action;
            std::cin >> action;
            std::cin.ignore(1000, '\n');
            packet.clear();

            if (action == 'c' || action == 'C') {
                messageType = MessageType::CHAT;
                std::cout << "Enter message: ";
                std::getline(std::cin, chatMessage);
                packet << messageType << chatMessage;
                if (client.send(packet) != sf::Socket::Status::Done) break;
            }
            else if (action == 'f' || action == 'F') {
                messageType = MessageType::FIRE_SHOT;

                bool validCoords = false;
                while (!validCoords) {
                    std::cout << "Enter Coordinates (X, Y): ";
                    if (getCoordinates(x_coord, y_coord)) {
                        if (x_coord >= 0 && x_coord < 10 && y_coord >= 0 && y_coord < 10) {
                            TileState existing = myPlayer.opponentBoard.getTileState(x_coord, y_coord);
                            if (existing == TileState::HIT || existing == TileState::MISS) {
                                std::cout << "Already fired there!\n";
                            } else {
                                validCoords = true;
                            }
                        } else {
                            std::cout << "Out of bounds (0-9).\n";
                        }
                    } else {
                        std::cout << "Invalid format.\n";
                    }
                }

                packet << messageType << x_coord << y_coord;
                if (client.send(packet) != sf::Socket::Status::Done) break;

                std::cout << "Waiting for reply...\n";
                if (client.receive(packet) != sf::Socket::Status::Done) break;

                int responseType = 0;
                // Receive: Type, Message, HitColor
                if (packet >> responseType >> responseString >> hitColor && responseType == MessageType::GAME_RESULT)
                {
                    std::cout << "Result: " << responseString << std::endl;
                    if (hitColor > 0) // It was a hit
                    {
                        myPlayer.opponentBoard.markHit(x_coord, y_coord, hitColor);
                        myPlayer.recordHit();
                    }
                    else
                    {
                        myPlayer.opponentBoard.markMiss(x_coord, y_coord);
                    }
                    waitForKey();
                }
                currentState = GameState::OPPONENTS_TURN;
            }
        }
        else // OPPONENTS_TURN
        {
            myPlayer.drawGameScreen();
            if (myPlayer.myBoard.isGameOver()) {
                std::cout << "\n*** YOU LOSE! ***\n";
                break;
            }

            std::cout << "\nWaiting for opponent...\n";
            if (client.receive(packet) != sf::Socket::Status::Done) break;

            if (!(packet >> messageType)) continue;

            if (messageType == MessageType::CHAT) {
                if (packet >> chatMessage) {
                    std::cout << "Chat: " << chatMessage << std::endl;
                    waitForKey();
                }
            }
            else if (messageType == MessageType::SETUP_COMPLETE) continue;
            else if (messageType == MessageType::FIRE_SHOT) {
                if (packet >> x_coord >> y_coord) {
                    std::cout << "Opponent fired at (" << x_coord << ", " << y_coord << ")\n";

                    TileState result = myPlayer.myBoard.checkShot(x_coord, y_coord);
                    hitColor = 0; // Default to 0 (Miss/No Color)

                    if (result == TileState::HIT)
                    {
                        hitColor = myPlayer.myBoard.getTileColor(x_coord, y_coord);
                        if (myPlayer.myBoard.isShipSunk(hitColor)) {
                             responseString = "You Sunk my " + getShipName(hitColor) + "!";
                        } else {
                             responseString = "You Hit!";
                        }
                    }
                    else
                    {
                        responseString = "You Missed!";
                    }

                    int responseType = MessageType::GAME_RESULT;
                    packet.clear();
                    // Send: Type, Message, HitColor
                    packet << responseType << responseString << hitColor;
                    if (client.send(packet) != sf::Socket::Status::Done) break;
                }
                currentState = GameState::MY_TURN;
            }
        }
    }
    waitForKey();
    return 0;
}