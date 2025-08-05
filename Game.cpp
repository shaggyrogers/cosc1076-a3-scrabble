#include "Game.h"

// Constructor
Game::Game() {
    this->running = true;

    // Set-up actions
    std::string helpMessage = std::string{
        "The following commands are available:\n\n"
        "pass                          Skip your turn.\n"
        "quit                          Quit the game.\n"
        "save {name}                   Save the game, creating a save file "
        "with the given name.\n"
        "replace {letter}              Replace a tile with one from the tile "
        "bag. The tile letter must be\n"
        "                              capitalised.\n"
        "place {letter} at {position}  Place one or more tiles onto the "
        "board. Positions have the format\n"
        "                              {row}{column} where row is a capital "
        "letter referring to a board row,\n"
        "                              and column is a number referring to a "
        "board column.\n"
        "place done                    Finish placing tiles.\n"
        "help                          Show this message."};

    this->possibleActions = std::vector<Action*>{
        new PassAction(),
        new QuitAction(),
        new SaveGameAction(this->saveGame),
        new ReplaceAction(),
        new PlaceAction(this->promptInputOnly, &this->running),
        new HelpAction(helpMessage)};
}

// Destructor
Game::~Game() {}

// Main loop.
void Game::run() {
    std::cout << "Welcome to Scrabble!" << std::endl;
    std::cout << "--------------------" << std::endl;

    int option = -1;

    while (this->running) {
        std::cout << std::endl;

        // Show menu
        option = this->getMenuOption();

        // Handle selected option
        if (option == MENU_OPTION_NEW_GAME) {
            // Perform new game sequence
            State* state = this->newGame();

            // state is nullptr if we can't read input from stdin
            if (state != nullptr) {
                this->runGame(state);

                // Simulate user selecting quit unless game ended because we
                // can't read from stdin
                if (this->running) {
                    option = MENU_OPTION_QUIT;
                }
            }
        } else if (option == MENU_OPTION_LOAD_GAME) {
            // Perform load game sequence
            State* state = nullptr;

            try {
                state = this->loadGame();
            } catch (std::invalid_argument& ex) {
                std::cout << "Failed to load save file (" << ex.what() << ")"
                          << std::endl;
            } catch (std::out_of_range& ex) {
                std::cout << "Failed to load save file (" << ex.what() << ")"
                          << std::endl;
            }

            // state is nullptr if we can't read input from stdin
            if (state != nullptr) {
                this->runGame(state);

                // Simulate user selecting quit unless game ended because we
                // can't read from stdin
                if (this->running) {
                    option = MENU_OPTION_QUIT;
                }
            }
        } else if (option == MENU_OPTION_CREDITS) {
            showCredits();
        }

        // Separate check here as we implement quitting by changing the
        // selected menu option above.
        if (option == MENU_OPTION_QUIT) {
            std::cout << "Bye!" << std::endl;
            this->running = false;
        }
    }
}

// Show the main menu. Prompt user to select an option and returns
// an integer corresponding to a value in MenuOptions.
// The menu is repeatedly displayed until a valid selection is made (or EOF)
MainMenuOption Game::getMenuOption() {
    int resp = -1;

    while (this->running &&
           (resp < MENU_OPTION_TERMINATE || resp > MENU_OPTION_QUIT)) {
        // Print menu
        std::cout << "Menu" << std::endl;
        std::cout << "----" << std::endl;
        std::cout << "1. New Game" << std::endl;
        std::cout << "2. Load Game" << std::endl;
        std::cout << "3. Credits" << std::endl;
        std::cout << "4. Quit" << std::endl << std::endl;

        // Read next line as response
        std::string line = this->promptInput();

        if (this->running) {
            // Parse response as integer
            try {
                resp = std::stoi(line);
            } catch (std::exception const&) {
                // std::invalid_argument and std::out_of_range can be
                // thrown.
            }
        } else {
            // EOF: Exit immediately
            resp = MENU_OPTION_TERMINATE;
        }

        // Inform user if selection was invalid
        if (resp < MENU_OPTION_TERMINATE || resp > MENU_OPTION_QUIT) {
            std::cout << "Invalid selection. Please enter a number from 1 to 4."
                      << std::endl
                      << std::endl;
        }
    }

    assert(resp >= MENU_OPTION_TERMINATE || resp <= MENU_OPTION_QUIT);
    return (MainMenuOption)resp;
}

// Display a list of contributor names, student IDs and email addresses.
void Game::showCredits() {
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Name: Angelica Celis" << std::endl;
    std::cout << "Student ID: 3600586" << std::endl;
    std::cout << "Email: s3600586@student.rmit.edu.au" << std::endl;
    std::cout << std::endl;
    std::cout << "Name: Michael De Pasquale" << std::endl;
    std::cout << "Student ID: 3935468" << std::endl;
    std::cout << "Email: s3935468@student.rmit.edu.au" << std::endl;
    std::cout << std::endl;
    std::cout << "Name: Yong Chuen Yeo" << std::endl;
    std::cout << "Student ID: 3718844" << std::endl;
    std::cout << "Email: s3718844@student.rmit.edu.au" << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

// Prompts the user for a line of input and returns it as a std::string.
// The running flag is set to false if no input is available, or the user
// presses Ctrl + D
std::string Game::promptInput() {
    std::string result = this->promptInputOnly();

    // Check for EOF, set running flag
    this->running = !std::cin.eof();

    return result;
}

std::string Game::promptInputOnly() {
    std::string resp;

    if (!std::cin.eof()) {
        // Show user prompt
        std::cout << "> ";

        // Read a line from stdin
        std::getline(std::cin, resp);
    }

    return resp;
}

// Prompt the user for a player name until a valid name is entered, which is
// then returned.
std::string Game::promptPlayerName(int playerNum) {
    std::string name("");

    while (this->running && !PlayerState::isValidPlayerName(name)) {
        std::cout << "Please enter a name for player " << playerNum
                  << std::endl;
        name = this->promptInput();

        if (this->running && !PlayerState::isValidPlayerName(name)) {
            std::cout << "Invalid name. Must be at least one character and"
                      << " consist of letters only." << std::endl
                      << std::endl;
        }
    }

    return name;
}

// Starts a new game, creating and returning a State*. If no input is
// available, returns nullptr.
State* Game::newGame() {
    // Print new game message
    std::cout << "Starting a New Game" << std::endl << std::endl;

    // Request player names
    std::string player1Name = this->promptPlayerName(1);
    std::string player2Name = this->promptPlayerName(2);

    // Player 2 name must be different to player 1 name, so ask again if the
    // names are the same. We also need to handle EOF.
    while (player2Name == player1Name && player1Name != "") {
        std::cout << "Player names must be different"
                  << " (please choose a name other than '" << player1Name
                  << "')" << std::endl;

        player2Name = this->promptPlayerName(2);
    }

    // Load scrabble tiles
    LinkedList* tileBag = State::loadScrabbleTiles(TILE_LIST_FILE);

    // Create game state
    State* state = nullptr;

    if (this->running) {
        std::cout << "Let's Play!" << std::endl << std::endl;

        // Construct state object
        state = new State(new PlayerState(player1Name, 0, new LinkedList()),
                          new PlayerState(player2Name, 0, new LinkedList()),
                          // Board is 15x15
                          new BoardState(15, 15), tileBag,
                          // Player 1 starts
                          0);

        // Shuffle tile bag
        state->shuffleTileBag();

        // Draw tiles for player hands
        state->getPlayer(0)->drawTiles(state->getTileBag());
        state->getPlayer(1)->drawTiles(state->getTileBag());
    }

    return state;
}

// Loads an existing save game file, returning a State*. If either no input
// is available or the user-specified save file could not be loaded, nullptr
// is returned.
State* Game::loadGame() {
    // Prompt user to enter filename
    std::cout << "Enter the filename from which to load a game" << std::endl
              << std::endl;
    std::string filename = this->promptInput();

    // Attempt to open file
    std::ifstream stream(filename);
    State* result = nullptr;

    if (stream.good()) {
        // File exists and can be read, attempt to load state
        result = State::fromStream(stream);
    } else if (this->running) {
        std::cout << "Save file '" << filename << "' does not exist or could"
                  << " not be read." << std::endl;
    }

    return result;
}

// Save state to a file with specified name. Returns true if successful, false
// otherwise.
bool Game::saveGame(State* state, std::string filename) {
    // Attempt to open file
    std::ofstream stream(filename);
    bool result = false;

    if (stream.good()) {
        state->toStream(stream);
        result = true;
    }

    return result;
}

// Run a game from the given game state.
void Game::runGame(State* state) {
    // TODO: State::isGameOver() -> need to store number of subsequent passes
    // TODO: BINGO! -> if player places all seven tiles to form a word, print
    // BINGO!!! and add 50 points

    assert(!state->getBoard()->isFull());
    assert(this->running);
    assert(!state->getDidQuit());
    assert(!this->isGameOver(state));

    // Loop until game over
    while (!this->isGameOver(state)) {
        // Display current player name
        PlayerState* player = state->getCurrentPlayer();
        std::cout << player->getName() << ", it's your turn" << std::endl;

        // Display player scores
        for (int i = 0; i < 2; i++) {
            std::cout << "Score for " << state->getPlayer(i)->getName() << ": "
                      << state->getPlayer(i)->getScore() << std::endl;
        }

        // Display board state
        std::cout << state->getBoard()->draw(true) << std::endl << std::endl;

        // Display current player hand
        std::cout << "Your hand is" << std::endl;
        std::cout << player->getHandString() << std::endl << std::endl;

        // Handle current player's turn
        this->runTurn(state);

        // End current player's turn
        state->endCurrentTurn();
    }

    // Game has ended - display end game message, scores and winner
    if (this->running) {
        // Display game over message and player scores
        std::cout << "Game over" << std::endl;

        // Display player scores
        for (int i = 0; i < 2; i++) {
            std::cout << "Score for " << state->getPlayer(i)->getName() << ": "
                      << state->getPlayer(i)->getScore() << std::endl;
        }

        // Display winner (or draw)
        if (state->getPlayer(0)->getScore() !=
            state->getPlayer(1)->getScore()) {
            // Winner exists, print name
            int winner = (state->getPlayer(0)->getScore() >
                                  state->getPlayer(1)->getScore()
                              ? 0
                              : 1);
            std::cout << "Player " << state->getPlayer(winner)->getName()
                      << " won!" << std::endl
                      << std::endl;
        } else {
            // Scores are equal - draw
            std::cout << "The game is a draw!" << std::endl << std::endl;
        }
    }

    return;
}

// Handle the current player's turn.
void Game::runTurn(State* state) {
    Action* action = nullptr;

    // Reads player input and find corresponding action
    // Prompt repeatedly until user enters valid input corresponding to
    // a possible action. Repeat again if the action does not end the
    // player's turn.
    while (!this->isGameOver(state) && action == nullptr) {
        std::string nextCmd = this->promptInput();

        // Loop through actions to find a match
        for (unsigned int i = 0;
             i < possibleActions.size() && action == nullptr; i++) {
            if (this->possibleActions[i]->isValid(nextCmd)) {
                action = this->possibleActions[i];
            }
        }

        if (this->running && action == nullptr) {
            // No valid action found
            std::cout << "Invalid Input: Not recognised as a valid command."
                      << std::endl
                      << "Use command 'help' for a list of available commands."
                      << std::endl;

        } else if (this->running) {
            // Valid action, perform
            bool didEndTurn = action->perform(nextCmd, state);

            // Set last action performed for player
            state->getCurrentPlayer()->setLastAction(action->getName());

            // Check if turn has ended
            if (!didEndTurn) {
                // Turn not ended - loop again
                action = nullptr;
            }
        }
    }
}

// Return true if the game should end, false otherwise.
bool Game::isGameOver(State* state) {
    return (
        // Not running (stdin EOF)
        !this->running
        // Board is full
        || state->getBoard()->isFull()
        // User has quit
        || state->getDidQuit()
        // Tile bag is empty and a player has no tiles in their hand
        || (state->getTileBag()->getLength() == 0 &&
            (state->getPlayer(0)->getNumTilesHeld() == 0 ||
             state->getPlayer(1)->getNumTilesHeld() == 0)));
}
