
#ifndef ASSIGN2_TILE_H
#define ASSIGN2_TILE_H

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string>

// Name of file containing all scrabble tiles
#define TILE_LIST_FILE "ScrabbleTiles.txt"

// Define a Letter type
typedef char Letter;

// Define a Value type
typedef int Value;

class Tile {
   public:
    // Constructor
    Tile(Letter letter, Value value);

    // Destructor
    ~Tile();

    // Copy constructor
    Tile(Tile& toCopy);

    // Create a Tile from its string representation, ignoring leading/trailing
    // whitespace. Supports both the format of lines in ScrabbleTiles.txt as
    // well as elements of the comma-separated lists found in saved game files.
    // i.e. will handle 'A-1', 'B 2', '  C-5 '. ' D 9 '
    // Throws std::invalid_argument if str could not be parsed.
    static Tile* fromString(std::string str);

    // The tile letter.
    Letter letter;

    // The score value.
    Value value;
};

#endif  // ASSIGN2_TILE_H
