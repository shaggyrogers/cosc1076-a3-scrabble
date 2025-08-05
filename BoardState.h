/*
  BoardState.h
  ============

  Description:           Represents the state of the scrabble board.

*/

#ifndef __BOARDSTATE_H__
#define __BOARDSTATE_H__

#include <assert.h>
#include <stdio.h>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "Tile.h"

class BoardState {
   public:
    // Constructor
    BoardState(int rows, int cols);

    // Destructor
    ~BoardState();

    // Read board state from an istream.
    // Throws std::invalid_argument or std::out_of_range if board state data is
    // invalid.
    static BoardState* fromStream(std::istream& stream);

    // Write board state to an ostream
    void toStream(std::ostream& stream);

    // Return a string representation of the board suitable for display to the
    // user. If termColour is true, add ANSI escape codes to colour the
    // result when displayed in a terminal.
    // Size must be between 1x1 and 26x99.
    std::string draw(bool termColour);

    // Return true if the board is empty, false otherwise.
    bool isEmpty();

    // Return true if the board is full, false otherwise.
    // This can only occur if the board size is reduced.
    bool isFull();

    // Parse a string of format {row_letter}{column_number} and set
    // rowIdx/colIdx to the appropriate row/column indices.
    // Returns true if successful and rowIdx/colIdx are non-negative, false
    // otherwise.
    static bool parseTilePosition(std::string data, int& rowIdx, int& colIdx);

    // Return true if the specified position is valid given the size of the
    // board.
    bool isValidPosition(int rowIdx, int colIdx);

    // Return true if a tile on the board has the specified position.
    bool tileExists(int rowIdx, int colIdx);

    // Return true if a tile is adjacent to the specified position (i.e.
    // manhattan distance of 1)
    bool tileIsAdjacentTo(int rowIdx, int colIdx);

    // Place a tile at the specified position.
    void placeTile(Tile* tile, int rowIdx, int colIdx);

    // Get the number of rows
    int getNumRows();

    // Get the number of columns
    int getNumCols();

    // Get a pointer to the tile at the specified row and column indices, or
    // nullptr if no tile exists. If the board does not contain the specified
    // position, std::invalid_argument is raised.
    Tile* getTileAt(int rowIdx, int colIdx);

   private:
    // Number of rows on the board
    int rows;

    // Number of columns on the board
    int cols;

    // State of tiles on the board
    std::vector<std::vector<Tile*>> tiles;

    // Parse a string of format {letter}@{row_letter}{column_number}, returning
    // a Tile* and setting rowIdx and colIdx to the appropriate row and column
    // index respectively.
    // Throws std::invalid_argument if data is malformed.
    static Tile* parseTileData(std::string data, int& rowIdx, int& colIdx);

    // Get ANSI escape for tile colour given row/column indices.
    std::string getTileColour(int rowIdx, int colIdx);
};

#endif /* __BOARDSTATE_H__ */
