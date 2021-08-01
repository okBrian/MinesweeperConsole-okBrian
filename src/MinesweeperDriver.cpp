#include "MinesweeperGame.h"

/*
argc - amount of command line argument
argv - array of each command line argument
*/
int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		showExitCode(1, "Usage: MinesweeperDriver SEED_FILE_PATH");
		return 1;
	}
	else
	{
		const std::string seedPath(argv[1]);
		MinesweeperGame game(seedPath);
		game.Game();
		if (game.exitCode == 2)
			return 2;
		else if (game.exitCode == 3)
			return 3;

	}

	return 0;
}
