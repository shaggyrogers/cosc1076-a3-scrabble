/*
  Action.h
  ========

  Description:           Classes representing possible actions.

*/

#include "Action.h"

// Constructor
Action::Action(std::regex matchExp) { this->matchExp = matchExp; }

// Destructor
Action::~Action() {}

// Returns true if the input (satisfying isMatch) is correctly formed
// and we can proceed to performing the action
bool Action::isValid(std::string line) {
    return std::regex_match(line, this->matchExp);
}

// Perform the action, returning true if the player's turn should end, false
// otherwise. Simply sets PlayerState->lastAction to tthis->getName() and
// returns true.
// When overriding in derived classes, this base method MUST be called to ensure
// that PlayerState->lastAction is set.
bool Action::perform(std::string line, State* state) {
    state->getCurrentPlayer()->setLastAction(this->getName());

    return true;
}

// Constructor for PlaceAction
PlaceAction::PlaceAction(promptInputCallback promptInputOnly, bool* running)
    : Action(std::regex("[Pp][Ll][Aa][Cc][Ee]\\s+([A-Z])\\s+[Aa][Tt]\\s+([A-Z]["
                        "0-9]+)\\s*$")) {
    this->promptInputOnly = promptInputOnly;
    this->running = running;
    this->placeDoneExp = std::regex("place\\s+done\\s*$",
                                    std::regex::ECMAScript | std::regex::icase);
    this->dictionary = Dictionary();
}

// Return the name of PlaceAction.
std::string PlaceAction::getName() { return "place"; }

// Prompt user to enter placements and construct a vector containing each,
// until either the player has no tiles or they enter 'place done'.
// After this, check that the set represents a valid scrabble move and
// if so perform it, otherwise display an error message.
bool PlaceAction::perform(std::string line, State* state) {
    std::vector<TilePlacement> placements;

    // Handle place command
    this->handlePlace(line, state, placements);

    // Next line
    line = this->promptInput();

    // Handle subsequent place commands and 'place done'
    while (*this->running && !std::regex_match(line, this->placeDoneExp)) {
        if (this->isValid(line)) {
            // Handle place command
            this->handlePlace(line, state, placements);
        } else {
            std::cout << "Invalid Input: Expected place command."
                      << " Enter 'place done' to finish placing tiles."
                      << std::endl;
        }

        // Next line
        line = this->promptInput();
    }

    // Check if move is valid
    bool turnEnds = false;
    std::string playWord;

    if (this->isValidMove(placements, state, playWord)) {
        // Perform move
        this->performMove(placements, state, playWord);

        // End turn
        turnEnds = true;
    } else {
        // Not valid - discard move and allow
        std::cout << "Invalid Input: Move is not valid." << std::endl;
    }

    return turnEnds;
}

// Parse line, construct a TilePlacement and add it to placements.
// Return true if successful, false otherwise. Does not check that the move is
// valid. Expects that line has format:
// 'place {letter} at {row_letter}{column_number}'
bool PlaceAction::handlePlace(std::string line, State* state,
                              std::vector<TilePlacement>& placements) {
    assert(this->isValid(line));

    // Extract tile letter
    // Must match here as we already matched in isValid().
    std::smatch match;
    assert(std::regex_match(line, match, this->matchExp));
    std::string letterStr = match[1];
    assert(letterStr.length() == 1);

    // Extract target position (shouldn't be possible to fail)
    int rowIdx, colIdx;
    std::string posStr = match[2];
    assert(BoardState::parseTilePosition(posStr, rowIdx, colIdx));

    // Construct move and add it to placements
    placements.push_back(TilePlacement(letterStr.data()[0], rowIdx, colIdx));

    return true;
}

// HACK: This reimplements Game::promptInput() so we can call it without being
// aware of Game
std::string PlaceAction::promptInput() {
    std::string result = this->promptInputOnly();

    // Check for EOF, set running flag
    *this->running = !std::cin.eof();

    return result;
}

// Return true if placements represents a valid scrabble move, false
// otherwise.
bool PlaceAction::isValidMove(std::vector<TilePlacement> placements,
                              State* state, std::string& word) {
    // FIXME: This is easier to implement with multiple return statements, but
    // this violates the style guide.

    // Need to check all of the following:
    // * The player's hand contains each specified tile (handle duplicates!)
    // * Either:
    //  - at least one tile placement is adjacent (V/H) to an existing tile, or
    //  - the board is empty
    // * All placements are either on the same row or column
    // * Each placement refers to an empty board position
    // * Each placement is next to another placement or an existing tile

    PlayerState* player = state->getCurrentPlayer();

    // Check player's hand contains all specified tiles
    PlayerState* playerCopy = new PlayerState(*player);

    for (unsigned int i = 0; i < placements.size(); i++) {
        if (playerCopy->removeTile(placements[i].letter) == nullptr) {
            delete playerCopy;

            // A tile wasn't found
            std::cout << "Invalid Input: Missing tile " << placements[i].letter
                      << std::endl;

            return false;
        }
    }

    // Check that the player is not attempting to place a tile over an existing
    // tile, or at an invalid location
    for (unsigned int i = 0; i < placements.size(); i++) {
        if (state->getBoard()->tileExists(placements[i].rowIdx,
                                          placements[i].colIdx)) {
            // Tile already exists at at least one placement position
            std::cout << "Invalid Input: Tile already exists at a specified "
                      << "position" << std::endl;

            return false;
        }

        if (!state->getBoard()->isValidPosition(placements[i].rowIdx,
                                                placements[i].colIdx)) {
            // Position is not valid
            std::cout << "Invalid Input: A specified position does not exist "
                         "on the board"
                      << std::endl;

            return false;
        }
    }

    // Check either board is empty, or at least one tile placement is adjacent
    // to an existing placement.
    if (!state->getBoard()->isEmpty()) {
        bool oneAdjacent = false;

        for (unsigned int i = 0; i < placements.size() && !oneAdjacent; i++) {
            oneAdjacent = state->getBoard()->tileIsAdjacentTo(
                placements[i].rowIdx, placements[i].colIdx);
        }

        if (!oneAdjacent) {
            std::cout << "Invalid Input: Board not empty and no adjacent tiles"
                      << std::endl;
            return false;
        }
    }

    // Check all placements are either on the same row or column
    TilePlacement firstPlacement = placements[0];
    bool sameRow = true;
    bool sameCol = true;

    for (unsigned int i = 1; i < placements.size() && (sameRow || sameCol);
         i++) {
        sameRow = (placements[i].rowIdx == firstPlacement.rowIdx);
        sameCol = (placements[i].colIdx == firstPlacement.colIdx);
    }

    if (!sameRow && !sameCol) {
        std::cout << "Invalid Input: Placements not on same row or column"
                  << std::endl;
        return false;
    }

    // Traverse the set of positions between min/max indices and check if
    // either a tile exists on the board, or a placement exists for each
    // position.
    // While doing this, also build up a string containing each letter after
    // placement so we can check whether the word is in the dictionary.
    std::vector<TilePlacement> placeRect =
        this->getIncludedPositions(state, placements);

    std::string placeWord;

    for (unsigned int i = 0; i < placeRect.size(); i++) {
        TilePlacement p = placeRect[i];
        TilePlacement toPlace = p;

        if (state->getBoard()->tileExists(p.rowIdx, p.colIdx)) {
            // 'hole' filled by tile from board. Add to placeWord.
            placeWord.append(
                1, state->getBoard()->getTileAt(p.rowIdx, p.colIdx)->letter);
        } else if (this->getPlacementIfExists(placements, p.rowIdx, p.colIdx,
                                              toPlace)) {
            // Tile to be placed at position. Add to placeWord.
            assert(toPlace.letter >= 'A' && toPlace.letter <= 'Z');
            placeWord.append(1, toPlace.letter);
        } else {
            // Blank space detected
            std::cout << "Invalid Input: A blank space exists on the board"
                      << " between tiles." << std::endl;

            return false;
        }
    }

    // Check if word to be placed is in the dictionary.
    if (!this->dictionary.contains(placeWord)) {
        std::cout << "Invalid Input: '" << placeWord
                  << "' is not in the scrabble dictionary." << std::endl;

        return false;
    } else {
        word = placeWord;
    }

    return true;
}

// Perform the move described by placements, assuming it is valid.
void PlaceAction::performMove(std::vector<TilePlacement> placements,
                              State* state, std::string word) {
    PlayerState* player = state->getCurrentPlayer();
    int incPoints = 0;

    // Check word is non-empty
    assert(word.length() > 0);

    // Place tiles and get score for move
    for (unsigned int i = 0; i < placements.size(); i++) {
        TilePlacement placement = placements[i];
        Tile* tile = player->removeTile(placement.letter);

        state->getBoard()->placeTile(tile, placement.rowIdx, placement.colIdx);
        incPoints += tile->value;
    }

    // Print played word and score
    std::cout << "Played word '" << word << "'"
              << " (+" << incPoints << " points)" << std::endl;
    player->raiseScore(incPoints);

    // Handle BINGO (player places all 7 tiles - +50 points)
    if (placements.size() == 7) {
        std::cout << "BINGO!!! (+50 points)" << std::endl << std::endl;
        player->raiseScore(50);
    }

    // Replenish player tiles
    player->drawTiles(state->getTileBag());
}

// Check if placements contains a tile placement at the specified position.
// If so, returns true and sets out to the placement, otherwise returns
// false.
bool PlaceAction::getPlacementIfExists(std::vector<TilePlacement> placements,
                                       int rowIdx, int colIdx,
                                       TilePlacement& out) {
    bool exists = false;

    for (unsigned int i = 0; i < placements.size() && !exists; i++) {
        exists =
            (placements[i].rowIdx == rowIdx && placements[i].colIdx == colIdx);

        if (exists) {
            out = placements[i];
        }
    }

    return exists;
}

// Return a set of TilePlacement instances that cover all the positions
// included in the word resulting from a set of placements from the player's
// tile bag, with order top left to bottom right.
// Includes board tile positions preceding or following the word if they
// exist. Does not check that there are no "holes" in the word.
std::vector<TilePlacement> PlaceAction::getIncludedPositions(
    State* state, std::vector<TilePlacement> placements) {
    // First, find min/max indices in the placement list.
    int minRowIdx = state->getBoard()->getNumRows();
    int minColIdx = state->getBoard()->getNumCols();
    int maxRowIdx = -1;
    int maxColIdx = -1;

    for (unsigned int i = 0; i < placements.size(); i++) {
        TilePlacement p = placements[i];

        if (p.rowIdx < minRowIdx) {
            minRowIdx = p.rowIdx;
        }

        if (p.colIdx < minColIdx) {
            minColIdx = p.colIdx;
        }

        if (p.rowIdx > maxRowIdx) {
            maxRowIdx = p.rowIdx;
        }

        if (p.colIdx > maxColIdx) {
            maxColIdx = p.colIdx;
        }
    }

    // Indices must describe a rectangle with a width or length of 1
    assert(minRowIdx == maxRowIdx || minColIdx == maxColIdx);

    // Get row/column offset to move 1 tile towards the bottom right in the
    // direction in which the tiles were placed (horizontal/vertical)
    int colOff = minColIdx == maxColIdx ? 0 : 1;
    int rowOff = minRowIdx == maxRowIdx ? 0 : 1;

    // DEBUGGING
    // std::cout << "box: (" << minRowIdx << ", " << minColIdx << ") -> ("
    //           << maxRowIdx << ", " << maxColIdx << ")" << std::endl;
    // std::cout << "Offset: (" << rowOff << ", " << colOff << ")" << std::endl;

    // Special case: If only one letter is placed, check for an adjacent tile
    // to attempt to determine direction. Default to horizontal if no tile is
    // found.
    // Note this won't properly handle the case where one letter is placed to
    // join a horizontal and vertical word.
    if (rowOff == 0 && colOff == 0) {
        if (state->getBoard()->tileExists(minColIdx, minRowIdx - 1) ||
            state->getBoard()->tileExists(minColIdx, minRowIdx + 1)) {
            // Vertical
            rowOff = 1;
        } else {
            // Horizontal
            colOff = 1;
        }
    }

    // May only move horizontal or vertical
    assert(rowOff + colOff == 1);

    // Extend min indices to include preceding tiles on the board
    while (
        state->getBoard()->tileExists(minRowIdx - rowOff, minColIdx - colOff)) {
        minRowIdx -= rowOff;
        minColIdx -= colOff;
    }

    // Extend max indices to include succeeding tiles on the board
    while (
        state->getBoard()->tileExists(maxRowIdx + rowOff, maxColIdx + colOff)) {
        maxRowIdx += rowOff;
        maxColIdx += colOff;
    }

    // Build ordered list of positions from the bounds of the rectangle
    // described by the min/max indices
    std::vector<TilePlacement> result;

    // First position
    result.push_back(TilePlacement('_', minRowIdx, minColIdx));

    while (minRowIdx != maxRowIdx || minColIdx != maxColIdx) {
        // Move one tile towards bottom right
        minColIdx += colOff;
        minRowIdx += rowOff;

        // Add position
        result.push_back(TilePlacement('_', minRowIdx, minColIdx));
    };

    return result;
}

// Constructor for ReplaceAction
ReplaceAction::ReplaceAction()
    : Action(std::regex("[Rr][Ee][Pp][Ll][Aa][Cc][Ee]\\s+([A-Z])\\s*$")) {}

// Return the name of ReplaceAction.
std::string ReplaceAction::getName() { return "replace"; }

// Replace the first tile with specified letter in the player's hand with the
// next tile from the tile bag.
bool ReplaceAction::perform(std::string line, State* state) {
    // Must match here as we already matched in isValid().
    std::smatch match;
    assert(std::regex_match(line, match, this->matchExp));

    // Extract tile letter
    std::string letterStr = match[1];
    assert(letterStr.length() == 1);
    Letter letter = letterStr.data()[0];

    // Remove tile from hand, if matching tile exists
    PlayerState* player = state->getCurrentPlayer();
    Tile* tile = player->removeTile(letter);
    bool turnEnds = false;

    // Check for match.
    if (tile != nullptr) {
        // Match found, add to tile bag and draw another tile
        state->getTileBag()->addBack(tile);
        int tilesDrawn = player->drawTiles(state->getTileBag());

        // We should always draw 0 or 1 tiles
        assert(tilesDrawn == 0 || tilesDrawn == 1);

        // End turn
        turnEnds = true;
    } else {
        // No match found
        std::cout << std::endl
                  << "Invalid Input: No tile '" << letter << "' found in hand."
                  << std::endl
                  << std::endl;
    }

    return turnEnds;
}

// Constructor for PassAction
PassAction::PassAction()
    : Action(std::regex("pass\\s*$",
                        std::regex::ECMAScript | std::regex::icase)) {}

// Return the name of PassAction.
std::string PassAction::getName() { return "pass"; }

// Do nothing and end the current player's turn. If the player passed on the
// previous turn and the tile bag is empty, this ends the game.
bool PassAction::perform(std::string line, State* state) {
    // End game if tile bag is empty and the player passed on their last turn
    if (state->getTileBag()->getLength() == 0 &&
        state->getCurrentPlayer()->getLastAction() == this->getName()) {
        state->quit();
    }

    // End turn
    return true;
}

// Save game constructor.
// Allow alphanumeric characters only - this should ensure that saving game
// doesn't allow a malicious user to write over arbitrary files outside of the
// current directory.
// Would probably be better to use a format like '.{name}.save' for save files.
SaveGameAction::SaveGameAction(saveGameCallback saveGame)
    : Action(std::regex("save\\s+([A-Za-z0-9.]+)\\s*$",
                        std::regex::ECMAScript | std::regex::icase)) {
    this->saveGame = saveGame;
}

// Return the name of SaveGameAction.
std::string SaveGameAction::getName() { return "save"; }

// Save the game. Does not end the player's turn.
bool SaveGameAction::perform(std::string line, State* state) {
    // Extract save file name
    // Must match here as we already matched in isValid().
    std::smatch match;
    assert(std::regex_match(line, match, this->matchExp));

    // Call saveGame() and display appropriate message
    if (this->saveGame(state, match[1])) {
        std::cout << std::endl
                  << "Game successfully saved" << std::endl
                  << std::endl;
    } else {
        std::cout << std::endl
                  << "Failed to save game!" << std::endl
                  << std::endl;
    }

    // Saving does not end turn.
    return false;
}

// Constructor for QuitAction
QuitAction::QuitAction()
    : Action(std::regex("quit\\s*$",
                        std::regex::ECMAScript | std::regex::icase)) {}

// Return the name of QuitAction.
std::string QuitAction::getName() { return "quit"; }

// End the game.
bool QuitAction::perform(std::string line, State* state) {
    state->quit();

    // Need to end turn to quit.
    return true;
}

// Constructor for HelpAction. Accepts help message to display.
HelpAction::HelpAction(std::string message)
    : Action(
          std::regex("help\\s*$", std::regex::ECMAScript | std::regex::icase))

{
    this->message = message;
}

// Return the name of this action.
std::string HelpAction::getName() { return "help"; }

// Display the help message. Does not end turn.
bool HelpAction::perform(std::string line, State* state) {
    // Print help message
    std::cout << this->message << std::endl << std::endl;

    // Don't end turn
    return false;
}
