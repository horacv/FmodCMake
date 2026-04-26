#include "app/app.h"

int main(int argc, char* argv[])
{
	Application game;
	game.Initialize();
	game.Run();
	game.Terminate();

	return 0;
}
