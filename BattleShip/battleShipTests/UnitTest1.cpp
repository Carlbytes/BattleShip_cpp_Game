#include "CppUnitTest.h"
#include "Player.hpp"
#include "Board.hpp"
#include "Ship.hpp"
#include "GameManager.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BattleshipTests
{
	//Board logic tests
    TEST_CLASS(BoardTests)
    {
    public:

        // Test 1: Check if a new board is actually empty
        TEST_METHOD(TestBoardInitialization)
        {
            Board board;
            // Check a few random spots
            Assert::IsTrue(board.getTileState(0, 0) == TileState::EMPTY, L"(0,0) should be empty");
            Assert::IsTrue(board.getTileState(9, 9) == TileState::EMPTY, L"(9,9) should be empty");
        }

        // Test 2: Valid Ship Placement
        TEST_METHOD(TestValidPlacement)
        {
            Board board;
            // Place size 3 horizontally at 0,0
            bool isValid = board.isValidPlacement(0, 0, 3, true);
            Assert::IsTrue(isValid, L"Placement at 0,0 size 3 should be valid");

            // Actually place it
            board.placeShip(0, 0, 3, true, 1);
            Assert::IsTrue(board.getTileState(0, 0) == TileState::SHIP);
            Assert::IsTrue(board.getTileState(2, 0) == TileState::SHIP);
        }

        // Test 3: Boundary Checks
        TEST_METHOD(TestOutOfBoundsPlacement)
        {
            Board board;
            // Try to place size 3 at 9,0 horizontally (would occupy 9, 10, 11)
            bool isValid = board.isValidPlacement(9, 0, 3, true);
            Assert::IsFalse(isValid, L"Placement going off right edge should fail");

            // Try to place size 3 at 0,9 vertically (would occupy 0,9; 0,10; 0,11)
            bool isValidVert = board.isValidPlacement(0, 9, 3, false);
            Assert::IsFalse(isValidVert, L"Placement going off bottom edge should fail");
        }

        // Test 4: Overlapping Ships (Crucial Rule!)
        TEST_METHOD(TestOverlappingPlacement)
        {
            Board board;
            // 1. Place a ship at 2,2 (Size 3, Horizontal) -> Occupies 2,2 | 3,2 | 4,2
            board.placeShip(2, 2, 3, true, 1);

            // 2. Try to place another ship crossing it at 3,1 (Size 3, Vertical) -> Would occupy 3,1 | 3,2 | 3,3
            bool isValid = board.isValidPlacement(3, 1, 3, false);

            Assert::IsFalse(isValid, L"Should not allow placing a ship on top of another");
        }

        // Test 5: Firing Mechanics
        TEST_METHOD(TestFiringAndSinking)
        {
            Board board;
            // Place a Size 2 ship (ID 5) at 0,0
            board.placeShip(0, 0, 2, true, 5);

            // 1. Hit first part
            TileState result1 = board.checkShot(0, 0);
            Assert::IsTrue(result1 == TileState::HIT, L"First shot should be HIT");

            // Check if sunk (Should be FALSE, only 1/2 hit)
            Assert::IsFalse(board.isShipSunk(5), L"Ship should not be sunk yet");

            // 2. Hit second part
            TileState result2 = board.checkShot(1, 0);
            Assert::IsTrue(result2 == TileState::HIT, L"Second shot should be HIT");

            // Check if sunk (Should be TRUE, 2/2 hit)
            Assert::IsTrue(board.isShipSunk(5), L"Ship should be sunk now");
        }
    };

	//Player logic tests here for initialization and scoring
    TEST_CLASS(PlayerTests)
    {
    public:

        // Test 6: Does the player start with the correct fleet?
        TEST_METHOD(TestFleetInitialization)
        {
            Player p;
            // Fleet size should be 5
            size_t count = p.fleet.size();
            Assert::AreEqual((size_t)5, count, L"Player should have 5 ships");

            // Total health should be 14 (4+3+3+2+2)
            Assert::AreEqual(14, p.totalShipHealth, L"Total ship health should be 14");
        }

        // Test 7: Scoring Mechanic
        TEST_METHOD(TestScoreTracking)
        {
            Player p;
            Assert::AreEqual(0, p.hitsScored, L"Score should start at 0");

            p.recordHit();
            p.recordHit();

            Assert::AreEqual(2, p.hitsScored, L"Score should increment to 2");
        }
    };

	//Ship polymorphism tests for all ship types
    TEST_CLASS(ShipTests)
    {
    public:
        // Test 8: Verify Polymorphism works for all ship types
        TEST_METHOD(TestShipPolymorphism)
        {
            // Battleship
            Battleship b;
            Assert::AreEqual("Battleship", b.getName().c_str());
            Assert::AreEqual(4, b.getSize());

            // Destroyer
            Destroyer d;
            Assert::AreEqual("Destroyer", d.getName().c_str());
            Assert::AreEqual(2, d.getSize());

            // Patrol Boat
            PatrolBoat p;
            Assert::AreEqual("Patrol Boat", p.getName().c_str());
            Assert::AreEqual(5, p.getColorId()); // 5 is Blue
        }
    };
}