#pragma once
#include <string>

// ABSTRACTION: Abstract Base Class
// We cannot create a "Ship" directly, only specific types of ships.
class Ship {
protected:
    std::string name;
    int size;
    int colorId; // Matches the Board's color system (1=Cyan, 2=Yellow, etc.)

public:
    virtual ~Ship() = default; // Virtual destructor for proper cleanup

    // POLYMORPHISM: Pure virtual function
    // Every ship MUST define its own name behavior.
    virtual std::string getName() const = 0;

    // Encapsulation: Getters for protected data
    int getSize() const { return size; }
    int getColorId() const { return colorId; }
};

// INHERITANCE: Derived Classes
class Battleship : public Ship {
public:
    Battleship() {
        name = "Battleship";
        size = 4;
        colorId = 1; // Cyan
    }
    std::string getName() const override { return name; }
};

class Cruiser : public Ship {
public:
    Cruiser() {
        name = "Cruiser";
        size = 3;
        colorId = 2; // Yellow
    }
    std::string getName() const override { return name; }
};

class Submarine : public Ship {
public:
    Submarine() {
        name = "Submarine";
        size = 3;
        colorId = 3; // Magenta
    }
    std::string getName() const override { return name; }
};

class Destroyer : public Ship {
public:
    Destroyer() {
        name = "Destroyer";
        size = 2;
        colorId = 4; // Green
    }
    std::string getName() const override { return name; }
};

class PatrolBoat : public Ship {
public:
    PatrolBoat() {
        name = "Patrol Boat";
        size = 2;
        colorId = 5; // Blue
    }
    std::string getName() const override { return name; }
};