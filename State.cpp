/*
  State.h
  =======

  Description:           Represents the game state.

*/

#include "State.h"

// Constructor for TilePlacement
TilePlacement::TilePlacement(Letter letter, int rowIdx, int colIdx) {
    this->letter = letter;
    this->rowIdx = rowIdx;
    this->colIdx = colIdx;
}

// Constructor
State::State(PlayerState* player1, PlayerState* player2, BoardState* board,
             LinkedList* tileBag, int playerTurnIdx) {
    // Pointers must not be null
    assert(player1 != nullptr && player2 != nullptr);
    assert(board != nullptr);
    assert(tileBag != nullptr);

    // Players must not have the same name, as this would cause ambiguity in
    // save files.
    if (player1->getName() == player2->getName()) {
        throw std::invalid_argument("Player names must not be the same");
    }

    // Only 2 players currently supported, so current player index must be 0/1
    if (playerTurnIdx != 0 && playerTurnIdx != 1) {
        throw std::invalid_argument(
            "Invalid player turn index, must be 0 or 1");
    }

    this->players[0] = player1;
    this->players[1] = player2;
    this->board = board;
    this->tileBag = tileBag;
    this->playerTurnIdx = playerTurnIdx;
    this->didQuit = false;
}

// Destructor
State::~State() {
    delete this->players[0];
    delete this->players[1];
    delete this->board;
    delete this->tileBag;
}

// Read game state from an istream
// Throws std::invalid_argument or std::out_of_range if state data is
// invalid.
State* State::fromStream(std::istream& stream) {
    // Stream state must be OK
    assert(stream.good());

    // Read player 1 data
    PlayerState* player1 = PlayerState::fromStream(stream);

    // Read player 2 data
    PlayerState* player2 = PlayerState::fromStream(stream);

    // Read board state
    BoardState* board = BoardState::fromStream(stream);

    // Read tile bag contents
    // NOTE: ScrabbleTiles.txt is 394 bytes. Given the format, 2048 bytes should
    // always be sufficient to read the tile list.
    std::string tileBagStr;
    std::getline(stream, tileBagStr);
    LinkedList* tileBag = LinkedList::fromString(tileBagStr);

    // Read current player name
    std::string curPlayerName;
    std::getline(stream, curPlayerName);

    // Determine player index from player name
    int playerTurnIdx = -1;

    if (player1->getName() == curPlayerName) {
        playerTurnIdx = 0;
    } else if (player2->getName() == curPlayerName) {
        playerTurnIdx = 1;
    }

    // Construct and return State
    return new State(player1, player2, board, tileBag, playerTurnIdx);
}

// Write game state to an ostream
void State::toStream(std::ostream& stream) {
    // Write player 1 data
    this->players[0]->toStream(stream);
    stream << std::endl;

    // Write player 2 data
    this->players[1]->toStream(stream);
    stream << std::endl;

    // Write board state
    this->board->toStream(stream);
    stream << std::endl;

    // Write tile bag contents
    stream << this->tileBag->toString();
    stream << std::endl;

    // Write current player name
    stream << this->players[this->playerTurnIdx]->getName();
}

LinkedList* State::loadScrabbleTiles(std::string path) {
    // Open file
    std::ifstream stream(path);
    assert(stream.is_open());
    assert(stream.good());

    LinkedList* result = new LinkedList();

    // Read tiles and construct LinkedList
    std::string tileStr;
    std::getline(stream, tileStr);

    while (stream.good() && tileStr.length() > 0) {
        result->addBack(Tile::fromString(tileStr));
        std::getline(stream, tileStr);
    }

    // Close file
    stream.close();

    return result;
}

// Shuffle tileBag in-place
void State::shuffleTileBag() {
    assert(this->tileBag != nullptr);

    // Copy tile bag contents
    LinkedList* tmp = new LinkedList(*this->tileBag);

    // Clear tile bag
    this->tileBag->clear();

    // Repopulate tile bag by removing randomly-selected elements from the copy
    while (tmp->getLength() > 0) {
        this->tileBag->addBack(tmp->remove(rand() % tmp->getLength()));
    }

    // Delete copy
    delete tmp;
}

// Returns the current player's state
PlayerState* State::getCurrentPlayer() {
    // Only 2 players supported
    assert(this->playerTurnIdx == 0 || this->playerTurnIdx == 1);

    return this->players[this->playerTurnIdx];
}

// Change to next player's turn
void State::endCurrentTurn() {
    this->playerTurnIdx = (this->playerTurnIdx + 1) % 2;
}

// Set didQuit flag to true.
void State::quit() { this->didQuit = true; }

// Return a pointer to the board state.
BoardState* State::getBoard() { return this->board; }

// Return a pointer to the tile bag.
LinkedList* State::getTileBag() { return this->tileBag; }

// Return the player with the given index. Currently must be 0 or 1 as
// only two players are suppored.
PlayerState* State::getPlayer(int idx) {
    // Ensure index is valid
    assert(idx == 0 || idx == 1);

    // Return target player
    return this->players[idx];
}

// Returns true if quit() has been called.
bool State::getDidQuit() { return this->didQuit; }
