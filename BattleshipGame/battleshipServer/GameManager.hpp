#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <optional>

// Shared Enums (Moved here so they are accessible)
enum MessageType { CHAT = 1, FIRE_SHOT = 2, GAME_RESULT = 3, SETUP_COMPLETE = 4, PASS_TURN = 5, COIN_FLIP = 6 };
enum GameState { MY_TURN, OPPONENTS_TURN };

class GameManager {
private:
    // 1. Private static instance
    static GameManager* instance;

    // 2. Private constructor (prevents "new GameManager()" from outside)
    GameManager();

    // Helper methods (Internal logic)
    void runGame(bool isServer);
    bool tryConnect(sf::TcpSocket& socket, const std::string& ipString, unsigned short port);
    bool getCoordinates(int& x, int& y);
    void waitForKey();
    std::string getShipName(int colorCode);

public:
    // 3. Public static accessor (The only way to get the object)
    static GameManager* getInstance();

    // The main entry point
    void run();
};