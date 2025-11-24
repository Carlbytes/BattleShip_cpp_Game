#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <optional>
#include "Player.hpp"

// Shared Enums
enum MessageType { CHAT = 1, FIRE_SHOT = 2, GAME_RESULT = 3, SETUP_COMPLETE = 4 };
enum GameState { MY_TURN, OPPONENTS_TURN };

// ==========================================
//           SHARED HELPER FUNCTIONS
// ==========================================

void waitForKey() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(1000, '\n'); // Clear any leftover input
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

// Helper for Client Connection
bool tryConnect(sf::TcpSocket& socket, const std::string& ipString, unsigned short port)
{
    std::cout << "Connecting to " << ipString << "...\n";
    std::optional<sf::IpAddress> serverIp = sf::IpAddress::resolve(ipString);
    if (!serverIp) return false;
    if (socket.connect(serverIp.value(), port) == sf::Socket::Status::Done) return true;
    return false;
}

// ==========================================
//           SERVER LOGIC
// ==========================================
void runServer()
{
    std::cout << "--- STARTING SERVER ---\n";

    sf::TcpListener listener;
    unsigned short port = 54000;

    if (listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "Error: Could not listen on port " << port << "\n";
        return;
    }
    std::cout << "Server listening on " << port << "...\n";

    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Status::Done) {
        std::cerr << "Error: Could not accept connection.\n";
        return;
    }
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
    int hitColor = 0;

    GameState currentState = GameState::OPPONENTS_TURN; // Server goes second

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
                            }
                            else {
                                validCoords = true;
                            }
                        }
                        else {
                            std::cout << "Out of bounds (0-9).\n";
                        }
                    }
                    else {
                        std::cout << "Invalid format.\n";
                    }
                }

                packet << messageType << x_coord << y_coord;
                if (client.send(packet) != sf::Socket::Status::Done) break;

                std::cout << "Waiting for reply...\n";
                if (client.receive(packet) != sf::Socket::Status::Done) break;

                int responseType = 0;
                if (packet >> responseType >> responseString >> hitColor && responseType == MessageType::GAME_RESULT)
                {
                    std::cout << "Result: " << responseString << std::endl;
                    if (hitColor > 0)
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
                    hitColor = 0;

                    if (result == TileState::HIT)
                    {
                        hitColor = myPlayer.myBoard.getTileColor(x_coord, y_coord);
                        if (myPlayer.myBoard.isShipSunk(hitColor)) {
                            responseString = "You Sunk my " + getShipName(hitColor) + "!";
                        }
                        else {
                            responseString = "You Hit!";
                        }
                    }
                    else
                    {
                        responseString = "You Missed!";
                    }

                    int responseType = MessageType::GAME_RESULT;
                    packet.clear();
                    packet << responseType << responseString << hitColor;
                    if (client.send(packet) != sf::Socket::Status::Done) break;
                }
                currentState = GameState::MY_TURN;
            }
        }
    }
    waitForKey();
}

// ==========================================
//           CLIENT LOGIC
// ==========================================
void runClient()
{
    std::cout << "--- STARTING CLIENT ---\n";

    sf::TcpSocket socket;
    unsigned short port = 54000;
    std::string ipAddressString;
    const std::string configFilename = "ip_config.txt";

    std::ifstream configFileIn(configFilename);
    if (configFileIn.is_open()) { std::getline(configFileIn, ipAddressString); configFileIn.close(); }
    else { ipAddressString = "127.0.0.1"; }

    bool isConnected = tryConnect(socket, ipAddressString, port);
    if (!isConnected) {
        std::cout << "Enter Server IP: ";
        std::cin >> ipAddressString;
        isConnected = tryConnect(socket, ipAddressString, port);
        if (isConnected) {
            std::ofstream out(configFilename);
            if (out.is_open()) { out << ipAddressString; out.close(); }
        }
    }
    if (!isConnected) return;

    std::cout << "Connected!\n";

    sf::Packet packet;
    Player myPlayer;
    myPlayer.setupBoard();

    // --- Sync ---
    std::cout << "Waiting for opponent...\n";
    packet << (int)MessageType::SETUP_COMPLETE << "Ready";
    socket.send(packet);

    bool opponentReady = false;
    while (!opponentReady) {
        packet.clear();
        if (socket.receive(packet) == sf::Socket::Status::Done) {
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
    int hitColor = 0;

    GameState currentState = GameState::MY_TURN; // Client starts first

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
                if (socket.send(packet) != sf::Socket::Status::Done) break;
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
                            }
                            else {
                                validCoords = true;
                            }
                        }
                        else {
                            std::cout << "Out of bounds.\n";
                        }
                    }
                    else {
                        std::cout << "Invalid format.\n";
                    }
                }

                packet << messageType << x_coord << y_coord;
                if (socket.send(packet) != sf::Socket::Status::Done) break;

                std::cout << "Waiting for reply...\n";
                if (socket.receive(packet) != sf::Socket::Status::Done) break;

                int responseType = 0;
                if (packet >> responseType >> responseString >> hitColor && responseType == MessageType::GAME_RESULT)
                {
                    std::cout << "Result: " << responseString << std::endl;
                    if (hitColor > 0)
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
            if (socket.receive(packet) != sf::Socket::Status::Done) break;

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
                    hitColor = 0;

                    if (result == TileState::HIT)
                    {
                        hitColor = myPlayer.myBoard.getTileColor(x_coord, y_coord);
                        if (myPlayer.myBoard.isShipSunk(hitColor)) {
                            responseString = "You Sunk my " + getShipName(hitColor) + "!";
                        }
                        else {
                            responseString = "You Hit!";
                        }
                    }
                    else
                    {
                        responseString = "You Missed!";
                    }

                    int responseType = MessageType::GAME_RESULT;
                    packet.clear();
                    packet << responseType << responseString << hitColor;
                    if (socket.send(packet) != sf::Socket::Status::Done) break;
                }
                currentState = GameState::MY_TURN;
            }
        }
    }
    waitForKey();
}

// ==========================================
//           MAIN MENU
// ==========================================
int main()
{
    std::cout << "=================================\n";
    std::cout << "    BATTLESHIP - CONSOLE EDITION \n";
    std::cout << "=================================\n";
    std::cout << "1. Host a Game (Server)\n";
    std::cout << "2. Join a Game (Client)\n";
    std::cout << "Select an option: ";

    char choice;
    std::cin >> choice;
    // Clear buffer so getline in logic doesn't skip
    std::cin.ignore(1000, '\n');

    if (choice == '1')
    {
        runServer();
    }
    else if (choice == '2')
    {
        runClient();
    }
    else
    {
        std::cout << "Invalid choice. Exiting.\n";
    }

    return 0;
}