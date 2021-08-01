#include "MinesweeperGame.h"

/* Checks if the os is Windows */
#ifdef _WIN32
#include <windows.h>
void Clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}
#else
void Clear()
{
	std::cout << std::string(100, '\n');
}
#endif

/* 
Constructor
Reads Seed File, Sets Grid, and Class Variables
*/
MinesweeperGame::MinesweeperGame(const std::string& filePath)
	: numBombs(0), flagPlaced(0), correctFlag(0), rounds(0), sizeWin(0), stateOfGame(gameState::Neutral), rows(0), cols(0), exitCode(0)
{
	/* Reserving atleast 20 bytes of data to the vector for the minimum 5 ints in the seed file */
	seedVals.reserve(5);

	std::ifstream scanner(filePath);
	std::string tempLine;
	std::string seedContent;

	/* if Seed File is openable */
	if (scanner)
	{
		/* Itterates through each line of the seed file */
		while (scanner.good())
		{
			std::getline(scanner, tempLine);
			seedContent.append(tempLine + ' ');
		}
	}
	else
	{
		exitCode = 2;
		showExitCode(exitCode, "Seed File Not Found Error: " + filePath);
		stateOfGame = gameState::Quit;
	}

	/* Removes White Space */
	std::stringstream ss_SeedVals(seedContent);

	/* stringstream converted to std::string */
	std::string s_SeedVals = ss_SeedVals.str();
	/* Checks to see if s_SeedVals has anything other than digits and whitespace ie - 'a', 'b', 'c' */
	if (s_SeedVals.find_first_not_of("0123456789 ") != std::string::npos)
	{
		exitCode = 3;
		showExitCode(3, "Seed File Malformed Error: Expected Token is not a Digit");
		stateOfGame = gameState::Quit;
	}


	
	/* If something is wrong game quits */
	if (stateOfGame != gameState::Quit)
	{

		/* Copy the strings from stringstream into integers inserted into std::vector seedVals */
		std::copy(std::istream_iterator<int>(ss_SeedVals), std::istream_iterator<int>(), std::back_inserter(seedVals));

		/* Setting Class Parameters for Clarity and Easy Use */
		rows = seedVals[0];
		cols = seedVals[1];
		numBombs = seedVals[2];
		sizeWin = rows * cols - numBombs;

		/* Error Checking */
		checkSeedFile();

		/* Makes empty sqaure */
		std::vector<std::string> tempVec;
		for (int i = 0; i < rows; i++)
			tempVec.push_back("   ");

		/* Makes 2d Grid for game */
		for (int i = 0; i < cols; i++)
		{
			grid.push_back(tempVec);
		}
		tempVec.clear();
	}

}

/* Deconstructor Clears Heap Allocated Vectors */
MinesweeperGame::~MinesweeperGame()
{
	
	for (int i = 0; i < cols; i++)
	{
		grid[i].clear();
	}

	grid.clear();
	revealedSquares.clear();
	bombCoords.clear();
	flagCoords.clear();
	seedVals.clear();
}

void MinesweeperGame::Game()
{
	if (stateOfGame != gameState::Quit)
	{
		printWelcome();
		std::cout << "Press Enter to continue" << std::endl;
		std::cin.ignore();
		Clear();

		/* From Seed File Gets Coords for Bombs */
		findBombs();
	}

	while (stateOfGame == gameState::Neutral)
	{
		stateOfGame = promptUser();

		if (stateOfGame == gameState::Loss)
			printLoss();

		else if (stateOfGame == gameState::Win)
			printWin();
	}
}

void MinesweeperGame::printWelcome()
{
	/* R - Raw Text outputting as is */
	std::cout << R"(
        _
  /\/\ (_)_ __   ___  _____      _____  ___ _ __   ___ _ __
 /    \| | '_ \ / _ \/ __\ \ /\ / / _ \/ _ \ '_ \ / _ \ '__|
/ /\/\ \ | | | |  __/\__ \\ V  V /  __/  __/ |_) |  __/ |
\/    \/_|_| |_|\___||___/ \_/\_/ \___|\___| .__/ \___|_|
                 A L P H A   E D I T I O N |_| v2020.sp
	)" << std::endl;
}

inline void MinesweeperGame::printWin()
{
	Clear();
	/* For Windows Cmd setting text output to Unicode 16 */
	if (_setmode(_fileno(stdout), _O_U16TEXT))
	{
	std::wcout << LR"(
 ░░░░░░░░░▄░░░░░░░░░░░░░░▄░░░░ "So Doge"
 ░░░░░░░░▌▒█░░░░░░░░░░░▄▀▒▌░░░
 ░░░░░░░░▌▒▒█░░░░░░░░▄▀▒▒▒▐░░░ "Such Score"
 ░░░░░░░▐▄▀▒▒▀▀▀▀▄▄▄▀▒▒▒▒▒▐░░░
 ░░░░░▄▄▀▒░▒▒▒▒▒▒▒▒▒█▒▒▄█▒▐░░░ "Much Minesweeping"
 ░░░▄▀▒▒▒░░░▒▒▒░░░▒▒▒▀██▀▒▌░░░
 ░░▐▒▒▒▄▄▒▒▒▒░░░▒▒▒▒▒▒▒▀▄▒▒▌░░ "Wow"
 ░░▌░░▌█▀▒▒▒▒▒▄▀█▄▒▒▒▒▒▒▒█▒▐░░
 ░▐░░░▒▒▒▒▒▒▒▒▌██▀▒▒░░░▒▒▒▀▄▌░
 ░▌░▒▄██▄▒▒▒▒▒▒▒▒▒░░░░░░▒▒▒▒▌░
 ▀▒▀▐▄█▄█▌▄░▀▒▒░░░░░░░░░░▒▒▒▐░
 ▐▒▒▐▀▐▀▒░▄▄▒▄▒▒▒▒▒▒░▒░▒░▒▒▒▒▌
 ▐▒▒▒▀▀▄▄▒▒▒▄▒▒▒▒▒▒▒▒░▒░▒░▒▒▐░
 ░▌▒▒▒▒▒▒▀▀▀▒▒▒▒▒▒░▒░▒░▒░▒▒▒▌░
 ░▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▒▄▒▒▐░░
 ░░▀▄▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▄▒▒▒▒▌░░
 ░░░░▀▄▒▒▒▒▒▒▒▒▒▒▄▄▄▀▒▒▒▒▄▀░░░ CONGRATULATIONS!
 ░░░░░░▀▄▄▄▄▄▄▀▀▀▒▒▒▒▒▄▄▀░░░░░ YOU HAVE WON!
 ░░░░░░░░░▒▒▒▒▒▒▒▒▒▒▀▀░░░░░░░░ SCORE:)";
		std::wcout << 100 * rows * cols / rounds;
	}
}

inline void MinesweeperGame::printLoss()
{
	Clear();
	std::cout << R"(
 Oh no... You revealed a mine!
  __ _  __ _ _ __ ___   ___    _____   _____ _ __
 / _` |/ _` | '_ ` _ \ / _ \  / _ \ \ / / _ \ '__|
| (_| | (_| | | | | | |  __/ | (_) \ V /  __/ |
 \__, |\__,_|_| |_| |_|\___|  \___/ \_/ \___|_|
 |___/
	)" << std::endl;
}

void MinesweeperGame::printMineField()
{
	std::cout << "Rounds Completed: " << rounds << "\n\n";

	/* Prints Grid and Beautifies it */
	for (unsigned int i = 0; i < grid.size(); i++)
	{
		std::cout << i << ' ';
		for (unsigned int j = 0; j < grid[i].size(); j++)
			std::cout << '|' << grid[i][j];
		std::cout << '|' << std::endl;
	}

	/* Prints the index numbers at the bottom */
	std::cout << "    ";
	for (unsigned int i = 0; i < grid[0].size(); i++)
		std::cout << i << "   ";

	std::cout << std::endl;
}

gameState MinesweeperGame::promptUser()
{
	Clear();
	printMineField();

	std::string input;
	std::cout << "minesweeper-alpha: ";
	std::getline(std::cin, input);

	/* Removes whitespace */
	std::stringstream ss_UserInput(input);

	std::string s_UserInput;
	 int y = 0;
	 int x = 0;
	 /* Puts each value seperated by whitespace into a std::string, and 2 ints */
	ss_UserInput >> s_UserInput >> y >> x;

	/* Transform the string to lower case letters */
	std::transform(s_UserInput.begin(), s_UserInput.end(), s_UserInput.begin(), ::tolower);

	/* Checks to see if the integers are in range of the grid */
	if ((y > rows - 1) || (x > cols - 1))
	{
		std::cout << "Invalid Command: " << input << "\nClick Enter to Continue ";
		std::cin.ignore();
		return gameState::Neutral;
	}
	else if ((s_UserInput == "reveal") || (s_UserInput == "r")) /* <1>, <2>, < > */
	{
		vec2 coord = { y, x };
		revealedSquares.push_back(coord);
		/* Removes Duplicate Revealed Square coords*/
		removeDupe(revealedSquares);

		/* Checks to see if player has won */
		if ((correctFlag == numBombs) && (revealedSquares.size() == sizeWin))
			return gameState::Win;

		rounds++;
		return checkRevealed(coord);
	}

	else if ((s_UserInput == "mark") || (s_UserInput == "m")) /* <F> */
	{
		vec2 coord = { y, x };
		grid[coord.y][coord.x] = "<F>";
		flagCoords.push_back(coord);
		removeDupe(flagCoords);
		/* Checks to see how many correct flags have been placed */
		if (flagPlaced >= numBombs - 1)
		{
			for (vec2 bombCoord : bombCoords)
				for (vec2 flagCoord : flagCoords)
					if ((flagCoord.y == bombCoord.y) && (flagCoord.x == bombCoord.x))
						correctFlag++;
		}


		if ((correctFlag == numBombs) && (revealedSquares.size() == sizeWin))
			return gameState::Win;

		rounds++;
		flagPlaced++;
		return gameState::Neutral;
	}

	else if ((s_UserInput == "guess") || (s_UserInput == "g"))
	{
		grid[y][x] = "<g>";
		rounds++;
		return gameState::Neutral;
	}
	else if ((s_UserInput == "help") || (s_UserInput == "h"))
	{
		std::cout << R"(
Commands Available...
 - Reveal: r/reveal row col
 - Mark: m/mark     row col
 - Guess: g/guess   row col
 - Help: h/help
 - Quit: q/quit

Click Enter to Continue
		)" << std::endl;
		std::cin.get();
		rounds++;
		return gameState::Neutral;
	}
	else if ((s_UserInput == "quit") || (s_UserInput == "q"))
	{
		std::cout << "Qutting the game...\nBye!" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return gameState::Quit;
	}
	else if (s_UserInput == "nofog") /* Cheat Commnad O.o */
	{
		for (vec2 coords : bombCoords)
		{
			if (grid[coords.y][coords.x] != "<F>")
				grid[coords.y][coords.x] = "< >";
		}
		rounds++;
		return gameState::Neutral;
	}
	/* If the string command was invalid */
	else
	{
		std::cout << "Invalid Command: " << input << "\nClick Enter to Continue ";
		std::cin.ignore();
		return gameState::Neutral;
	}
}
/* Goes through the SeedVals to find the bombCoords */
void MinesweeperGame::findBombs()
{
	const int vecSize = seedVals[2] * 2;
	bombCoords.reserve(vecSize);
	int ind1 = 3;
	int ind2 = 4;
	vec2 coord = { seedVals[ind1] , seedVals[ind2] };

	for ( int i = 0; i < seedVals[2]; i++)
	{
		coord.y = seedVals[ind1];
		coord.x = seedVals[ind2];
		bombCoords.push_back(coord);
		ind1 += 2;
		ind2 += 2;
	}

	/* Lamda (anon function) Checks to see if any one of the vec2 coords are larger than the rows or cols */
	auto check = 
		/*Captures - class variables rows, and cols */[&rows = MinesweeperGame::rows, &cols = MinesweeperGame::cols]
		/*Parameter vec2 Coords from BombCoords */	  (const vec2& coords)
		/* If coord.y >= rows or coord.x >= cols */		{return coords.y >= rows || coords.x >= cols; };

		/* Lambdas are normally formatted like this - [capture](parameter) { code here; }; */

	/* Check if any Bomb Coordinates are too large */
	if (std::any_of(bombCoords.begin(), bombCoords.end(), check))
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Bomb Coordinates too large ");
		stateOfGame = gameState::Quit;
	}

}

gameState MinesweeperGame::checkRevealed(vec2& coord)
{
	int bombCount = 0;
	for (vec2 bombCoord : bombCoords)
	{
		/* If revealed bomb loss */
		if ((coord.y == bombCoord.y) && (coord.x == bombCoord.x))
			return gameState::Loss;

		constexpr vec2 offset[8] = 
		{ 
			{+1, 0}, {-1, 0}, { 0,+1}, { 0,-1}, 
			{-1,-1}, {+1,-1}, {-1,+1}, {+1,+1} 
		};

		/* Itterates through each offset value to up, up-right, right, down-right, down, down-left, left, up-left */
		for (int i = 0; i < 8; i++)
		{
			surrondingBomb(coord.y + offset[i].y , coord.x + offset[i].x, bombCoord, bombCount);
		}
	}
	if (bombCount > 0)
	{
		grid[coord.y][coord.x] = '<' + std::to_string(bombCount) + '>';
		return gameState::Neutral;
	}
	grid[coord.y][coord.x] = "< >";
	return gameState::Neutral;
}

void MinesweeperGame::surrondingBomb(const int& y, const int& x, vec2& bombCoord, int& bombCount)
{
	if ((y == bombCoord.y) && (x == bombCoord.x))
		bombCount++;
}

/* Removes Duplicate vec2 from a vector */
void MinesweeperGame::removeDupe(std::vector<vec2>& vecList)
{
	/* Sorts the list from any 2 vec2 in the same vector from greatest to least I think */
	auto less = [](const vec2& vec, const vec2& vec1) {return vec.x < vec1.x && vec.y < vec1.y; };

	/* Checks for any equal vec2 in the vector */
	auto equal = [](const vec2& vec, const vec2& vec1) {return vec.x == vec1.x && vec.y == vec1.y; };

	std::sort(vecList.begin(), vecList.end(), less);
	/* std::unique finds the extra vec2 if they are sorted ie - {1,2}{1,2} removes 1 {1,2}, but {1,2}{1,1}{1,2} nothing gets removed */
	auto last = std::unique(vecList.begin(), vecList.end(), equal);
	/* Actually erases the extra vec2 in vector */
	vecList.erase(last, vecList.end());
}

void showExitCode(const int& exitCode, const std::string& msg)
{
	std::cout  << msg << "\nExit : " << exitCode << "\nClick Enter to Continue";
	std::cin.get();
}

void MinesweeperGame::checkSeedFile()
{
	/* Checks for even amount of bombCoords */
	if ((seedVals.size() - 3) % 2 != 0)
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Not Enough Bomb Coordinates");
		stateOfGame = gameState::Quit;
	}
	else if (rows < 5)
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Not Enough Rows");
		stateOfGame = gameState::Quit;
	}
	else if (rows > 10)
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Too Many Rows");
		stateOfGame = gameState::Quit;
	}
	else if (cols < 5)
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Not Enough Columns");
		stateOfGame = gameState::Quit;
	}
	else if (cols > 10)
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Too Many Columns");
		stateOfGame = gameState::Quit;
	}
	else if (numBombs < 1)
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Not Enough Mines");
		stateOfGame = gameState::Quit;
	}
	else if (numBombs > (rows * cols - 1))
	{
		exitCode = 3;
		showExitCode(exitCode, "Seed File Malformed Error: Too Many Mines");
		stateOfGame = gameState::Quit;
	}

}



/*
Recusion :/
DOESN'T WORK MIGHT LOOK INTO LATER

PROBLEM - 
INFINTE LOOP IF COORD HAS BOMB NEARBY 
GETS STUCK TOO OFTEN

void MinesweeperGame::recursion(vec2& coord)
{
	vec2 offset[8] =
	{
		{-1, 0}, { 0,+1}, {-1,+1}, {+1,+1},
		{ 0,-1}, {+1, 0}, {+1,-1}, {-1,-1}
	};

	int bombCount = 0;
	for (vec2 bombCoord : bombCoords)
	{
		for (int i = 0; i < 8; i++)
		{
			surrondingBomb(coord.y + offset[i].y, coord.x + offset[i].x, bombCoord, bombCount);
		}
	}
	if (bombCount == 0)
	{
		if (grid[coord.y][coord.x] == "   ")
			grid[coord.y][coord.x] = "< >";
			Clear();
			printMineField();
		{
			for (int i = 0; i < 8; i++)
			{
				if (coord.y + offset[i].y != rows)
					if (coord.x + offset[i].x != cols)
					{
						if ((coord.y + offset[i].y >= 0) && (coord.x + offset[i].x >= 0) && (grid[coord.y + offset[i].y][coord.x + offset[i].x] == "   "))
							recursion(coord = { coord.y + offset[i].y, coord.x + offset[i].x });
					}
			}
		}
	}

}
*/