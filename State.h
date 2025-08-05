/*
  State.h
  =======

  Description:           Represents the game state.

*/

#ifndef __STATE_H__
#define __STATE_H__

#include <assert.h>
#include <stdio.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "BoardState.h"
#include "LinkedList.h"
#include "PlayerState.h"
#include "Tile.h"

// FIXME: make members private and add getters/setters
// FIXME: Move to separate files, or OK together?

// Stores the action of placing a tile before it is performed.
struct TilePlacement {
   public:
    TilePlacement(Letter letter, int rowIdx, int colIdx);

    Letter letter;
    int rowIdx;
    int colIdx;
};

class State {
   public:
    // Constructor
    State(PlayerState* player1, PlayerState* player2, BoardState* board,
          LinkedList* tileBag, int playerTurnIdx);

    // Destructor
    ~State();

    // Read game state from an istream
    // Throws std::invalid_argument or std::out_of_range if state data is
    // invalid.
    static State* fromStream(std::istream& stream);

    // Write game state to an ostream
    void toStream(std::ostream& stream);

    // Load the list of tiles from ScrabbleTiles.txt
    static LinkedList* loadScrabbleTiles(std::string path);

    // Shuffle tileBag in-place
    void shuffleTileBag();

    // Returns the current player's state
    PlayerState* getCurrentPlayer();

    // Change to next player's turn
    void endCurrentTurn();

    // Set didQuit flag to true.
    void quit();

    // Return a pointer to the board state.
    BoardState* getBoard();

    // Return a pointer to the tile bag.
    LinkedList* getTileBag();

    // Return the player with the given index. Currently must be 0 or 1 as
    // only two players are suppored.
    PlayerState* getPlayer(int idx);

    // Returns true if quit() has been called.
    bool getDidQuit();

   private:
    // The state of the scrabble board
    BoardState* board;

    // The state of the tile bag
    LinkedList* tileBag;

    // The state of both players
    PlayerState* players[2];

    // The index of the player whose turn it is
    int playerTurnIdx;

    // true if we should quit the game, false otherwise.
    bool didQuit;
};

#endif /* __STATE_H__ */
