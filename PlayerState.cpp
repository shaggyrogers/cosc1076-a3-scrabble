/*
  PlayerState.cpp
  ===============

  Description:           Represents the player state.

*/

#include "PlayerState.h"

// Constructor
PlayerState::PlayerState(std::string name, int score, LinkedList* hand) {
    // Name must be valid
    if (!PlayerState::isValidPlayerName(name)) {
        std::string what("Invalid player name: '");
        what.append(name);
        what.append("'");
        throw std::invalid_argument(what);
    }

    // Score must be valid
    if (score < 0) {
        throw std::invalid_argument(
            std::string("Invalid player score (must be positive)"));
    }

    // hand must not be null
    assert(hand != nullptr);

    this->name = name;
    this->score = score;
    this->hand = hand;
    this->lastAction = "";
}

// Copy constructor
PlayerState::PlayerState(PlayerState& state)
    : PlayerState(state.name, state.score, new LinkedList(*state.hand)) {}

// Destructor
PlayerState::~PlayerState() { delete this->hand; }

// Read player state from an istream.
// Throws std::invalid_argument or std::out_of_range if player state data is
// invalid.
PlayerState* PlayerState::fromStream(std::istream& stream) {
    assert(stream.good());

    // Read player name
    std::string name;
    std::getline(stream, name);

    // Read player score
    std::string scoreStr;
    std::getline(stream, scoreStr);
    int score = std::stoi(scoreStr);

    // Read player hand
    std::string handStr;
    std::getline(stream, handStr);
    LinkedList* hand = LinkedList::fromString(handStr);

    // Create PlayerState using data read from stream
    return new PlayerState(name, score, hand);
}

// Write player state to an ostream
void PlayerState::toStream(std::ostream& stream) {
    stream << this->name << std::endl;
    stream << this->score << std::endl;
    stream << this->hand->toString();
}

// Returns true if the specified player name is valid, false otherwise.
// Valid names have length > 1 and consist of letters only.
bool PlayerState::isValidPlayerName(std::string name) {
    // Player name is immediately declared invalid if it is blank.
    // Otherwise, loop over each character, checking if each is a letter.
    bool valid = name.length() > 0;

    for (unsigned int i = 0; i < name.length() && valid; i++) {
        valid = ((name[i] >= 'a' && name[i] <= 'z') ||
                 (name[i] >= 'A' && name[i] <= 'Z'));
    }

    return valid;
}

// Draw tiles from tileBag until hand contains 7 tiles, or tileBag is
// empty.
// Returns the number of tiles drawn.
int PlayerState::drawTiles(LinkedList* tileBag) {
    int drawCount;

    for (drawCount = 0; this->hand->getLength() < 7 && tileBag->getLength() > 0;
         drawCount++) {
        this->hand->addBack(tileBag->remove(0));
    }

    // Player must not have more than 7 tiles
    assert(this->hand->getLength() <= 7);

    return drawCount;
}

// Attempt to remove the first tile matching the specified letter from the
// player's hand. Returns a Tile* if successful, nullptr otherwise.
Tile* PlayerState::removeTile(Letter letter) {
    Tile* result = nullptr;

    // Loop through hand, stopping early if we find the target tile
    for (int i = 0; i < this->hand->getLength() && result == nullptr; i++) {
        if (this->hand->get(i)->letter == letter) {
            // Match found, remove tile
            result = this->hand->remove(i);
        }
    }

    return result;
}

// Get score
int PlayerState::getScore() { return this->score; }

// Increase score by the provided integer value
void PlayerState::raiseScore(int by) { this->score += by; }

// Get the player's name.
std::string PlayerState::getName() { return this->name; }

// Return a comma-separated, ordered list of tiles in the player's hand,
// suitable for display.
std::string PlayerState::getHandString() { return this->hand->toString(); }

// Return tthe number of tiles in the player's hand.
int PlayerState::getNumTilesHeld() { return this->hand->getLength(); }

// Get the name of the last action performed, or an empty string if either the
// player has not yet performed an action since starting or loading the game.
std::string PlayerState::getLastAction() { return this->lastAction; }

// Set the name of the last action performed.
void PlayerState::setLastAction(std::string action) {
    this->lastAction = action;
}
