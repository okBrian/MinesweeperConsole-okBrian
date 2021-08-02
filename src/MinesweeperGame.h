#pragma once

/* Libraries Used in the Stream */
#include <iostream> /* cout, cin, wcout */
#include <fstream> /* fstream getline, istream iterators */

/* Used to remove whitespace in string and converting strings to ints */
#include <sstream> /* stringstream */

/* For Windows Cmd to allow unicode chars like ░ */
#include <io.h> /* _setmode */
/* Specify what type of Unicode in this case U16 */
#include <fcntl.h> /* _O_U16TEXT */

/* For pausing */
#include <thread> /* std::this_thread::sleep_for() */

#include <chrono> /* std::chrono::milliseconds */
#include <algorithm> /* std::transform(), std::sort(), std::unique(), std::any_of */
#include <string> /* std::string, std::find_first_not_of, ::npos */

/* For itterating in algorithm functions*/
#include <iterator> /* std::istream_iterator */

/* Container for data */
#include <vector> /* std::vector */

/* Enum used to track the gameState */
enum class gameState
{
	Loss = -1, Neutral = 0, Win = 1, Quit = 2
};

/* struct storing 2 int to represent the grid y,x | rows,cols coords */
struct vec2
{
	/* rows,cols / y,x */
	int y, x;
};

/* Minesweeper Class containing all game functions */
class MinesweeperGame
{
private:
	/*Class vars initialized on line 34 */
	int numBombs, flagPlaced, correctFlag, rounds;
	unsigned int sizeWin;
	bool cheat;
	gameState stateOfGame;
	/* 2d Vec grid */
	std::vector<std::vector<std::string>> grid;
	/* Store what sqaures the user has revealed */
	std::vector<vec2> revealedSquares;
	/* Stores the coords for important objects */
	std::vector<vec2> bombCoords; 
	std::vector<vec2> flagCoords;
	/* Stores the read values from the seed file */
	std::vector<int> seedVals;

public:
	 int rows, cols ,exitCode;
	/* Constructor that takes a string value defined in as a command line argument */
	MinesweeperGame(const std::string& seedPath);
	/* Deconstructor to clear heap allocated Vectors */
	~MinesweeperGame();
	/* Main Game Function*/
	void Game();
	
	inline void printWelcome();
	inline void printWin();
	inline void printLoss();

	void printMineField();

private:
	gameState promptUser();
	/* Finds bombs from seed file and stores them in a vector */
	void findBombs();
	/* Check if the revealed bomb has a bomb or should contain a number ex- <1>, <2> */
	gameState checkRevealed(vec2& coord);
	/* Checks a coord if it contains a bomb */
	void surrondingBomb(const int& x, const int& y, vec2& bombCoord, int& bombCount);
	/* Remove Duplicates in vectors containg vec2 */
	void removeDupe(std::vector<vec2>& vecList);
	/* Checks for Seed File Malformation */
	void checkSeedFile();
};

void showExitCode(const int& exitCode, const std::string& msg);