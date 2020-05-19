#define WIN32_LEAN_AND_MEAN
#include "Tetris.h"


int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hpprevinstance,
	LPSTR lpcmdline,
	int ncmdshow){


	Tetris tetris(hinstance);
	tetris.Run();

	return 0;
}

