/*
  Game.h
  ======

  Description:           Implements game logic.

*/

#ifndef __GAME_H__
#define __GAME_H__

#include <fstream>
#include <string>
#include <vector>

#include "Action.h"
#include "State.h"

// Menu options
enum MainMenuOption {
    // Returned after EOF
    MENU_OPTION_TERMINATE = 0,

    MENU_OPTION_NEW_GAME = 1,
    MENU_OPTION_LOAD_GAME = 2,
    MENU_OPTION_CREDITS = 3,
    MENU_OPTION_QUIT = 4,
};

class Game {
   public:
    // Constructor
    Game();

    // Destructor
    ~Game();

    // Main loop.
    void run();

   private:
    // Display a list of contributor names, student IDs and email addresses.
    void showCredits();

    // Show the main menu. Returns a MainMenuOption.
    MainMenuOption getMenuOption();

    // Prompts the user for a line of input and returns it as a std::string.
    // The running flag is set to false if either no input is available, or the
    // user presses Ctrl +
    std::string promptInput();

    // Prompt the user for input (see promptInput) but does not set the
    // 'running' flag. Hack to get PlaceAction working.
    static std::string promptInputOnly();

    // Prompt the user for a player name until a valid name is entered, which is
    // then returned.
    std::string promptPlayerName(int playerNum);

    // Starts a new game, creating and returning a State object.
    State* newGame();

    // Loads an existing save game file. Returning a State object.
    State* loadGame();

    // Save state to a file with specified name. Returns true if successful,
    // false otherwise.
    static bool saveGame(State* state, std::string filename);

    // Run a game from the given game state.
    void runGame(State* state);

    // Handle the current player's turn
    void runTurn(State* state);

    // Return true if the game should end, false otherwise.
    bool isGameOver(State* state);

    // If false, terminate immediately.
    bool running;

    // Set of possible actions
    std::vector<Action*> possibleActions;
};

#endif /* __GAME_H__ */
