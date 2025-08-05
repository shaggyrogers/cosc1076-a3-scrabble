
#include "Tile.h"

// Constructor
Tile::Tile(Letter letter, Value value) {
    // letter must be a valid uppercase letter
    assert(letter >= 'A' && letter <= 'Z');

    // Value must be positive
    assert(value >= 0);

    this->letter = letter;
    this->value = value;
}

// Destructor
Tile::~Tile() {}

// Copy constructor
Tile::Tile(Tile& toCopy) : Tile(toCopy.letter, toCopy.value) {}

// Create a Tile from its string representation, ignoring leading/trailing
// whitespace. Supports both the format of lines in ScrabbleTiles.txt as well
// as elements of the comma-separated lists found in saved game files.
// i.e. will handle 'A-1', 'B 2', '  C-5 '. ' D 9 '
// Throws std::invalid_argument if str could not be parsed.
Tile* Tile::fromString(std::string str) {
    const char* cStr = str.data();

    // Skip leading whitespace
    while (cStr[0] == ' ') {
        cStr++;
    }

    // Read values from string - handle both '-' or ' ' as separator
    Letter letter;
    Value value;

    if (sscanf(cStr, "%c%*[ -]%i", &letter, &value) != 2) {
        // Failure to parse
        std::string what("Unable to parse tile string '");
        what.append(cStr);
        what.append("'");

        throw std::invalid_argument(what);
    }

    return new Tile(letter, value);
}
