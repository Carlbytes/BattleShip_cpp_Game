#include "CppUnitTest.h"
#include "C:\CollegeStuff\Y3 Repos\SftDevGameProject\BattleShip\BattleShip\Board.hpp" // Adjust path if needed
#include "C:\CollegeStuff\Y3 Repos\SftDevGameProject\BattleShip\BattleShip\Ship.hpp"  // Adjust path if needed
// Note: You don't need to #include .cpp files, just headers.

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BattleshipTests
{
	TEST_CLASS(BoardTests)
	{
	public:

		// Test 1: Check if a new board is actually empty
		TEST_METHOD(TestBoardInitialization)
		{
			Board board;
			// Check random spot (0,0)
			bool isEmpty = (board.getTileState(0, 0) == TileState::EMPTY);
			Assert::IsTrue(isEmpty, L"Board (0,0) should be empty on init");
		}

		// Test 2: Valid Ship Placement
		TEST_METHOD(TestValidPlacement)
		{
			Board board;
			// Try to place a size 3 ship at 0,0 horizontally
			// Should return TRUE (valid)
			bool isValid = board.isValidPlacement(0, 0, 3, true);
			Assert::IsTrue(isValid, L"Placement at 0,0 size 3 should be valid");
		}

		// Test 3: Invalid Placement (Out of bounds)
		TEST_METHOD(TestOutOfBoundsPlacement)
		{
			Board board;
			// Try to place size 3 at 9,0 horizontally (would go to 9, 10, 11)
			// Should return FALSE
			bool isValid = board.isValidPlacement(9, 0, 3, true);
			Assert::IsFalse(isValid, L"Placement going out of bounds should be invalid");
		}

		// Test 4: Firing a Shot (Hit Mechanic)
		TEST_METHOD(TestFiringShot)
		{
			Board board;
			// Force place a ship manually for testing
			board.placeShip(5, 5, 1, true, 1);

			// Check Hit
			TileState result = board.checkShot(5, 5);
			Assert::IsTrue(result == TileState::HIT, L"Shooting at ship should return HIT");

			// Check Miss
			TileState missResult = board.checkShot(0, 0);
			Assert::IsTrue(missResult == TileState::MISS, L"Shooting at empty space should return MISS");
		}
	};

	TEST_CLASS(ShipTests)
	{
	public:
		// Test 5: Polymorphism Check
		TEST_METHOD(TestShipNames)
		{
			// Test that a Battleship returns "Battleship"
			Battleship b;
			std::string name = b.getName();
			// Assert::AreEqual doesn't like std::string, compare C-strings
			Assert::AreEqual("Battleship", name.c_str());

			Submarine s;
			Assert::AreEqual("Submarine", s.getName().c_str());
		}
	};
}