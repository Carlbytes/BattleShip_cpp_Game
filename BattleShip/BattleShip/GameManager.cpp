#include "GameManager.hpp"
#include "Player.hpp"
#include <iostream> // For std::cout, std::cin
#include <sstream> // For stringstream
#include <fstream> // For file I/O
#include <cstdlib> // For rand()
#include <ctime>   // For time()

// --- Cross-Platform Input Handling (Local to this file) ---
#ifdef _WIN32
#include <conio.h>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_SPACE 32
#define KEY_R 114

static int getKeyPress() {
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        return _getch(); // Extended code
    }
    return ch;
}

static void clearScreen() {
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
#define KEY_SPACE 32
#define KEY_R 114

static int getKeyPress() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    if (ch == 27) { getchar(); ch = getchar(); }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
static void clearScreen() { system("clear"); }
#endif
// ----------------------------------------------------------

//Initialize the static instance to null
GameManager* GameManager::instance = nullptr;

//Private Constructor
GameManager::GameManager() {
    //Seed random number generator once at startup
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

//Singleton Accessor
GameManager* GameManager::getInstance() {
    if (instance == nullptr) {
        instance = new GameManager();
    }
    return instance;
}

// ==========================================
//           SHARED HELPER FUNCTIONS
// ==========================================

//Method to wait for user input
void GameManager::waitForKey() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(1000, '\n'); //Clear buffer
    std::cin.get();
}

//Method to get coordinates
bool GameManager::getCoordinates(int& x, int& y) {
    std::string line;
    std::getline(std::cin, line);
    for (char& c : line) {
        if (c == ',') c = ' ';
    }
    std::stringstream ss(line);
    if (ss >> x >> y) return true;
    return false;
}

//Get the ship name based on ID
std::string GameManager::getShipName(int colorCode) {
    switch (colorCode) {
    case 1: return "Battleship (1x4)";
    case 2: return "Cruiser (1x3)";
    case 3: return "Submarine (1x3)";
    case 4: return "Destroyer (1x2)";
    case 5: return "Patrol Boat (1x2)";
    default: return "Ship";
    }
}

//Helper for Client Connection
bool GameManager::tryConnect(sf::TcpSocket& socket, const std::string& ipString, unsigned short port) {
    //Connect to server IP address and port 
    std::cout << "Connecting to " << ipString << "...\n";
    std::optional<sf::IpAddress> serverIp = sf::IpAddress::resolve(ipString);
    if (!serverIp) return false;
    if (socket.connect(serverIp.value(), port) == sf::Socket::Status::Done) return true;
    return false;
}

// ==========================================
//           CORE GAME LOGIC
// ==========================================

//Game logic method
void GameManager::runGame(bool isServer) {
    sf::TcpListener listener;
    sf::TcpSocket socket;
    unsigned short port = 54000;
    std::string ipAddressString;
    const std::string configFilename = "ip_config.txt";

    //Connection setup

    //Declare myName here so it survives the if/else block
    std::string myName;

    if (isServer) {
        std::cout << "--- HOSTING GAME ---\n";

        std::cout << "Enter your Username: ";
        std::cin >> myName;
        std::cin.ignore(1000, '\n');

        std::optional<sf::IpAddress> localIp = sf::IpAddress::getLocalAddress();
        if (localIp) {
            std::cout << "--------------------------------------\n";
            std::cout << "YOUR IP ADDRESS IS: " << localIp.value().toString() << "\n";
            std::cout << "Tell your friend to connect to this IP!\n";
            std::cout << "--------------------------------------\n";
        }

        if (listener.listen(port) != sf::Socket::Status::Done) {
            std::cerr << "Error: Could not listen on port " << port << "\n";
            return;
        }
        std::cout << "Waiting for friend to connect...\n";
        if (listener.accept(socket) != sf::Socket::Status::Done) return;
        std::cout << "Friend connected!\n";
    }
    else {
        //Client connection logic
        std::cout << "--- JOINING GAME ---\n";

        std::cout << "Enter your Username: ";
        std::cin >> myName;
        std::cin.ignore(1000, '\n');

        //Try to read last used IP from config file
        std::ifstream configFileIn(configFilename);
        //If file opens, read IP, else default to localhost
        if (configFileIn.is_open()) { std::getline(configFileIn, ipAddressString); configFileIn.close(); }
        else { ipAddressString = "127.0.0.1"; }

        //Attempt connection to stored IP
        bool isConnected = tryConnect(socket, ipAddressString, port);
        //If that fails, prompt user for IP until successful connection
        if (!isConnected) {
            std::cout << "Enter Host IP: ";
            std::cin >> ipAddressString;
            isConnected = tryConnect(socket, ipAddressString, port);
            if (isConnected) {
                std::ofstream out(configFilename);
                if (out.is_open()) { out << ipAddressString; out.close(); }
            }
        }
        //If still not connected, exit
        if (!isConnected) return;
        std::cout << "Connected!\n";
    }

    //Game Setup
    sf::Packet packet;
    Player myPlayer;
    myPlayer.setupBoard();

    //Sync and names
    std::cout << "Waiting for opponent to finish setup...\n";

    //Send the actual 'myName' variable
    packet << (int)MessageType::SETUP_COMPLETE << myName;
    socket.send(packet);

    std::string opponentName = "Opponent";
    bool opponentReady = false;
    //Loop until opponent is ready
    while (!opponentReady) {
        packet.clear();
        if (socket.receive(packet) == sf::Socket::Status::Done) {
            int type;
            if (packet >> type >> opponentName && type == MessageType::SETUP_COMPLETE) {
                opponentReady = true;
            }
        }
    }

    //Coin flip logic
    std::cout << "Flipping coin to decide who starts...\n";
    GameState currentState = GameState::OPPONENTS_TURN;
    bool iGoFirst = false;

    if (isServer) {
        int coin = std::rand() % 2;
        iGoFirst = (coin == 0);
        packet.clear();
        packet << (int)MessageType::COIN_FLIP << iGoFirst;
        socket.send(packet);

        if (iGoFirst) std::cout << "HEADS! You go first.\n";
        else          std::cout << "TAILS! " << opponentName << " goes first.\n";
    }
    else {
        packet.clear();
        if (socket.receive(packet) == sf::Socket::Status::Done) {
            int type;
            bool serverGoesFirst;
            if (packet >> type >> serverGoesFirst && type == MessageType::COIN_FLIP) {
                iGoFirst = !serverGoesFirst;
                if (iGoFirst) std::cout << "TAILS! You go first.\n";
                else          std::cout << "HEADS! " << opponentName << " goes first.\n";
            }
        }
    }

    currentState = iGoFirst ? GameState::MY_TURN : GameState::OPPONENTS_TURN;
    waitForKey();

    //Game Loop
    int messageType;
    std::string chatMessage;
    int x_coord, y_coord;
    std::string responseString;
    int hitColor = 0;

    while (true) {
        if (currentState == GameState::MY_TURN) {
            myPlayer.drawGameScreen();
            std::cout << " playing vs " << opponentName << "\n";

            if (myPlayer.hitsScored >= myPlayer.totalShipHealth) {
                std::cout << "\n*** YOU WIN! ***\n";
                break;
            }

            std::cout << "\nMY TURN: (c) Chat, (f) Fire, (p) Pass Turn: ";
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
            else if (action == 'p' || action == 'P') {
                messageType = MessageType::PASS_TURN;
                packet << messageType << "Passing Turn";
                socket.send(packet);
                std::cout << "Turn passed.\n";
                currentState = GameState::OPPONENTS_TURN;
            }
            else if (action == 'f' || action == 'F') {
                messageType = MessageType::FIRE_SHOT;

                // --- NEW CURSOR BASED TARGETING ---
                int cursorX = 0;
                int cursorY = 0;
                bool targetSelected = false;

                while (!targetSelected) {
                    // Valid if we haven't shot there yet (EMPTY or SHIP means we haven't hit/missed yet)
                    // Note: On opponent board, SHIP state is hidden, so initially everything looks EMPTY.
                    // If we previously fired, it would be HIT or MISS.
                    TileState ts = myPlayer.opponentBoard.getTileState(cursorX, cursorY);
                    bool validTarget = (ts == TileState::EMPTY || ts == TileState::SHIP);

                    // Use the Board's cursor display (Reuse the ship placement visualizer!)
                    // showShips=false (don't see enemy), size=1, horizontal=true (doesn't matter for 1x1)
                    myPlayer.opponentBoard.displayWithCursor(false, cursorX, cursorY, 1, true, validTarget);

                    // UPDATED PROMPT HERE:
                    std::cout << "\nSelect where to fire: WASD/ARROWS to move, ENTER/SPACE to Shoot\n";
                    int key = getKeyPress();

                    switch (key) {
                    case KEY_UP: case 'w': case 'W': if (cursorY > 0) cursorY--; break;
                    case KEY_DOWN: case 's': case 'S': if (cursorY < 9) cursorY++; break;
                    case KEY_LEFT: case 'a': case 'A': if (cursorX > 0) cursorX--; break;
                    case KEY_RIGHT: case 'd': case 'D': if (cursorX < 9) cursorX++; break;
                    case KEY_ENTER: case ' ':
                        if (validTarget) {
                            x_coord = cursorX;
                            y_coord = cursorY;
                            targetSelected = true;
                        }
                        break;
                    }
                }
                // ----------------------------------

                packet << messageType << x_coord << y_coord;
                if (socket.send(packet) != sf::Socket::Status::Done) break;

                std::cout << "Firing at " << x_coord << "," << y_coord << "... Waiting for reply...\n";
                if (socket.receive(packet) != sf::Socket::Status::Done) break;

                int responseType = 0;
                if (packet >> responseType >> responseString >> hitColor && responseType == MessageType::GAME_RESULT) {
                    std::cout << "Result: " << responseString << std::endl;
                    if (hitColor > 0) {
                        myPlayer.opponentBoard.markHit(x_coord, y_coord, hitColor);
                        myPlayer.recordHit();
                    }
                    else {
                        myPlayer.opponentBoard.markMiss(x_coord, y_coord);
                    }
                    waitForKey();
                }
                currentState = GameState::OPPONENTS_TURN;
            }
        }
        else { //OPPONENTS_TURN
            myPlayer.drawGameScreen();
            std::cout << " playing vs " << opponentName << "\n";

            if (myPlayer.myBoard.isGameOver()) {
                std::cout << "\n*** YOU LOSE! ***\n";
                break;
            }

            std::cout << "\nWaiting for " << opponentName << " to move...\n";
            if (socket.receive(packet) != sf::Socket::Status::Done) break;

            if (!(packet >> messageType)) continue;

            if (messageType == MessageType::CHAT) {
                if (packet >> chatMessage) {
                    std::cout << "\n[CHAT] " << opponentName << ": " << chatMessage << std::endl;
                    std::cout << "(Press Enter...)";
                    std::string dummy; std::getline(std::cin, dummy);
                }
            }
            else if (messageType == MessageType::PASS_TURN) {
                std::cout << "\n" << opponentName << " passed their turn!\n";
                waitForKey();
                currentState = GameState::MY_TURN;
            }
            else if (messageType == MessageType::FIRE_SHOT) {
                if (packet >> x_coord >> y_coord) {
                    std::cout << opponentName << " fired at (" << x_coord << ", " << y_coord << ")\n";
                    TileState result = myPlayer.myBoard.checkShot(x_coord, y_coord);
                    hitColor = 0;

                    if (result == TileState::HIT) {
                        hitColor = myPlayer.myBoard.getTileColor(x_coord, y_coord);
                        if (myPlayer.myBoard.isShipSunk(hitColor)) responseString = "You Sunk my " + getShipName(hitColor) + "!";
                        else responseString = "You Hit!";
                    }
                    else responseString = "You Missed!";

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

//Main Menu method
void GameManager::run() {
    while (true) {
        //Simple clear to keep menu clean
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::cout << "=================================\n";
        std::cout << "          BATTLESHIP      \n";
        std::cout << "=================================\n";
        std::cout << "1. Host a Game (Server)\n";
        std::cout << "2. Join a Game (Client)\n";
        std::cout << "3. Exit\n";
        std::cout << "Select an option: ";

        char choice;
        std::cin >> choice;
        std::cin.ignore(1000, '\n');

        if (choice == '1') {
            runGame(true);
        }
        else if (choice == '2') {
            runGame(false);
        }
        else if (choice == '3') {
            break;
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}