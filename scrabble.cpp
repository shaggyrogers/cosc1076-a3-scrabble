/*
  scrabble.cpp
  ============

  Description:           Entry point for the application.

*/

#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Game.h"

#define EXIT_SUCCESS 0

int main(void) {
    // Seed random number generator with the current time (as seconds since
    // unix epoch)
    srand(time(NULL));

    // Create a Game instance and invoke main loop
    Game game;
    game.run();

    return EXIT_SUCCESS;
}
