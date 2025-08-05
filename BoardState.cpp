/*
  BoardState.h
  ============

  Description:           Represents the state of the scrabble board.

*/

#include "BoardState.h"
#include "ANSI.h"

// Constructor
BoardState::BoardState(int rows, int cols) {
    // Board must have at least one tile slot
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument(std::string(
            "Invalid board size, number of rows and columns must be positive"));
    }

    // Limit maximum board size to ensure display() will not fail
    assert(rows <= 26 && cols < 99);
    if (rows >= 26 || cols > 99) {
        throw std::invalid_argument(
            std::string("Board size too large, must be at most 26x99"));
    }

    this->rows = rows;
    this->cols = cols;

    // Initialise vectors - set size
    this->tiles.resize(rows);

    for (int i = 0; i < rows; i++) {
        this->tiles[i].resize(cols);
    }

    // Board is blank
    // FIXME: is this necessary? check
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            this->tiles[row][col] = nullptr;
        }
    }
}

// Destructor
BoardState::~BoardState() {
    // Delete all tiles
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            delete this->tiles[row][col];
        }
    }
}

// Read board state from an istream.
// Throws std::invalid_argument or std::out_of_range if board state data is
// invalid.
BoardState* BoardState::fromStream(std::istream& stream) {
    assert(stream.good());

    // Read line and copy to temporary stringstream
    std::stringstream lineStream("");
    std::string line;
    std::getline(stream, line);
    lineStream << line;

    // Read row count
    char buf[8192];
    int rows, cols;
    lineStream.getline(buf, sizeof(buf) - 1, ',');
    rows = std::stoi(buf);

    // read column countt
    lineStream.getline(buf, sizeof(buf) - 1, ',');
    cols = std::stoi(buf);

    // Create blank board
    BoardState* board = new BoardState(rows, cols);

    // Fill blank board using tile data from stream
    while (lineStream.good()) {
        // Read tile data for next tile
        char buf[1024];
        lineStream.getline(buf, sizeof(buf) - 1, ',');

        // Parse tile data and set row in position.
        int rowIdx, colIdx;
        Tile* tile =
            BoardState::parseTileData(std::string(buf), rowIdx, colIdx);

        // Row/column indices must refer to a valid position
        if (rowIdx >= board->rows || colIdx > board->cols) {
            throw std::invalid_argument(
                "Row or column exceeds the size of the board");
        }

        board->tiles[rowIdx][colIdx] = tile;
    }

    return board;
}

// Parse a string of format {letter}@{row_letter}{column_number}, returning
// a Tile* and setting rowIdx and colIdx to the appropriate row and column
// index respectively.
// Ensures that the tile letter is valid and that rowIdx/colIdx are
// non-negative.
// Tile always has value 0.
// Throws std::invalid_argument if data is malformed.
Tile* BoardState::parseTileData(std::string data, int& rowIdx, int& colIdx) {
    // Create regex for parsing tile data/position strings
    // Format {letter}@{row_letter}{column_number}
    std::regex tileExp("[ ]*([A-Z])@([A-Z][0-9]+)");
    std::smatch match;
    std::regex_match(data, match, tileExp);

    // Expect 3 matches (string, letter, position)
    if (match.size() != 3) {
        std::string what("Malformed board tile string '");
        what.append(data);
        what.append("'");
        throw std::invalid_argument(what);
    }

    // Get each component
    std::string tileLetterStr = match[1];
    std::string tilePosStr = match[2];

    // Parse tile position and letter, throw invalid_argument if either is
    // invalid Note that Tile::Tile() will throw invalid_argument if the tile
    // letter is invalid.
    if (!BoardState::parseTilePosition(tilePosStr, rowIdx, colIdx) ||
        tileLetterStr.length() != 1) {
        std::string what("Unable to parse board tile string '");
        what.append(data);
        what.append("'");
        throw std::invalid_argument(what);
    }

    return new Tile(tileLetterStr.data()[0], 0);
}

// Write board state to an ostream
// Format
// ------
// {rows}, {cols}, {tile->letter}@{row_letter}{col_number}, ...
//
// For example, for the following 2x2 board:
//
//     0     1
// ---------------
// A | A-1 |     |
// B |     | B-2 |
//
// The result would be:
//
// 2, 2, A@A1, B@B2
void BoardState::toStream(std::ostream& stream) {
    // Write row, column counts
    stream << this->rows << ", " << this->cols;

    // Write list of tiles and their positions
    for (int row = 0; row < this->rows; row++) {
        for (int col = 0; col < this->cols; col++) {
            if (this->tiles[row][col] != nullptr) {
                Tile* tile = this->tiles[row][col];

                // Write tile data to stream
                stream << ", " << tile->letter << '@' << (char)('A' + row)
                       << col + 1;
            }
        }
    }
}

// Return a string representation of the board suitable for display to the
// user. If termColour is true, add ANSI escape codes to colour the
// result when displayed in a terminal.
// Size must be between 1x1 and 26x99.
std::string BoardState::draw(bool termColour) {
    // 3 spaces between numbers
    // 3 spaces per cell
    // example:
    //     0   1   2   3   4   5
    //   -------------------------
    // A |   |   |   |   |   |   |
    // B |   |   |   | W | I | N |
    // C |   | C |   | A |   |   |
    // D | P | L | A | Y | S |   |
    // E |   | A |   |   |   |   |
    // F |   | Y |   |   |   |   |

    std::stringstream stream("");

    // Add column headings
    stream << " ";

    for (int col = 0; col < this->cols; col++) {
        // NOTE: Breaks if this->cols > 99. This should not be the case as it
        // would violate the assertion in constructor.
        assert(col <= 99);
        stream << std::string(3 - (col > 9 ? 1 : 0), ' ') << col + 1;
    }

    // Top border (no. dashes = 1 + 4 * cols)
    stream << std::endl << "  ";

    for (int i = 0; i < 1 + 4 * this->cols; i++) {
        stream << '-';
    }

    // Rows & tiles
    for (int row = 0; row < this->rows; row++) {
        stream << std::endl;

        // Row heading and left border
        // NOTE: Breaks if this->rows > 26. This should not be the case as it
        // would violate the assertion in constructor.
        assert(row <= 26);

        // Row letters
        stream << (char)('A' + row);

        // Left border
        stream << " |";

        // Tiles and right border
        for (int col = 0; col < this->cols; col++) {
            Tile* tile = this->tiles[row][col];

            // Colour (if enabled)
            if (termColour) {
                stream << this->getTileColour(row, col);
            }

            // Letter or space
            stream << ' ' << (tile == nullptr ? ' ' : tile->letter);

            // Reset colour (if colour enabled)
            if (termColour) {
                stream << ANSI::Reset;
            }

            stream << " |";
        }
    }

    return stream.str();
}

// Return true if the board is empty, false otherwise.
bool BoardState::isEmpty() {
    bool empty = true;

    // Check each position for a tile, ending when we reach the end of the
    // board or a tile is found.
    for (int row = 0; row < this->rows && empty; row++) {
        for (int col = 0; col < this->cols && empty; col++) {
            empty = this->tiles[row][col] == nullptr;
        }
    }

    return empty;
}

// Return true if the board is full, false otherwise.
// This can only occur if the board size is reduced.
bool BoardState::isFull() {
    bool full = true;

    // Loop until we find a blank tile
    for (int row = 0; row < this->rows && full; row++) {
        for (int col = 0; col < this->cols && full; col++) {
            full = this->tiles[row][col] != nullptr;
        }
    }

    return full;
}

// Parse a string of format {row_letter}{column_number} and set
// rowIdx/colIdx to the appropriate row/column indices.
// Returns true if successful and rowIdx/colIdx are non-negative, false
// otherwise.
bool BoardState::parseTilePosition(std::string data, int& rowIdx, int& colIdx) {
    // Create regex for parsing tile data/position strings
    // Format {letter}@{row_letter}{column_number}
    std::regex posExp("([A-Z])([0-9]+)");
    std::smatch match;

    // Match input string
    std::regex_match(data, match, posExp);

    rowIdx = -1;
    colIdx = -1;

    // Expect 3 matches (string, row, col)
    if (match.size() == 3) {
        // Get components
        std::string rowLetterStr = match[1];
        std::string colNumStr = match[2];

        // Parse column number
        colIdx = std::stoi(colNumStr) - 1;

        // Parse row number (if valid)
        if (rowLetterStr.length() == 1 && rowLetterStr.data()[0] >= 'A' &&
            rowLetterStr.data()[0] <= 'Z') {
            rowIdx = (int)(rowLetterStr.data()[0] - 'A');
        }
    }

    return (rowIdx >= 0 && colIdx >= 0);
}

// Return true if the specified position is valid given the size of the
// board.
bool BoardState::isValidPosition(int rowIdx, int colIdx) {
    return (rowIdx >= 0 && rowIdx < this->rows && colIdx >= 0 &&
            colIdx < this->cols);
}

// Return true if a tile on the board has the specified position.
bool BoardState::tileExists(int rowIdx, int colIdx) {
    return (this->isValidPosition(rowIdx, colIdx) &&
            this->tiles[rowIdx][colIdx] != nullptr);
}

// Return true if a tile is adjacent to the specified position (i.e.
// manhattan distance of 1), false otherwise.
// Also returns false if the position is invalid.
bool BoardState::tileIsAdjacentTo(int rowIdx, int colIdx) {
    // Check left/right/up/down for a tile
    return (this->isValidPosition(rowIdx, colIdx) &&
            (this->tileExists(rowIdx, colIdx - 1) ||
             this->tileExists(rowIdx, colIdx + 1) ||
             this->tileExists(rowIdx - 1, colIdx) ||
             this->tileExists(rowIdx + 1, colIdx)));
}

// Place a tile at the specified position.
void BoardState::placeTile(Tile* tile, int rowIdx, int colIdx) {
    // Tile must not exist at position
    assert(!this->tileExists(rowIdx, colIdx));

    // Position must be valid
    assert(this->isValidPosition(rowIdx, colIdx));

    // Place tile
    this->tiles[rowIdx][colIdx] = tile;
}

// Get the number of rows
int BoardState::getNumRows() { return this->rows; }

// Get the number of columns
int BoardState::getNumCols() { return this->cols; }

// Get a pointer to the tile at the specified row and column indices, or
// nullptr if no tile exists. If the board does not contain the specified
// position, std::invalid_argument is raised.
Tile* BoardState::getTileAt(int rowIdx, int colIdx) {
    // Validate position
    if (!this->isValidPosition(rowIdx, colIdx)) {
        throw std::invalid_argument("Board position (" +
                                    std::to_string(rowIdx) + ", " +
                                    std::to_string(colIdx) + ") is invalid!");
    }

    // Return tile or nullptr
    return this->tiles[rowIdx][colIdx];
}

// Get ANSI escape for tile colour given row/column indices.
std::string BoardState::getTileColour(int rowIdx, int colIdx) {
    if (colIdx % 2) {
        return (rowIdx % 2 ? ANSI::MagentaFG : ANSI::BlueFG);
    } else {
        return (rowIdx % 2 ? ANSI::RedFG : ANSI::GreenFG);
    }
}
