#include "GameManager.hpp"

int main()
{
    // Singleton Pattern Usage
    GameManager::getInstance()->run();
    return 0;
}