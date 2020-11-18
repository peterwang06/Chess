#include <iostream>
#include "game.h"
#include "piece.h"
#include <SFML/Graphics.hpp>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define new new
#endif

int main() {
	game* chess = new game;
	delete chess;
	//_CrtDumpMemoryLeaks();
	return 0;
}