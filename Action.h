/*
  Action.h
  ========

  Description:           Classes representing possible actions.

*/

#ifndef __ACTION_H__
#define __ACTION_H__

#include <cstdio>
#include <map>
#include <regex>
#include <string>

#include "Dictionary.h"
#include "State.h"
#include "Tile.h"

// Typedef for saveGame() pointer
typedef bool (*saveGameCallback)(State*, std::string);

// Typedef for promptInput() pointer
typedef std::string (*promptInputCallback)(void);

// Base class for possible actions/commands
class Action {
   public:
    // Constructor
    Action(std::regex matchExp);

    // Destructor
    ~Action();

    // Returns true if the input corresponds to performing this action
    // and we can proceed to performing the action
    bool isValid(std::string line);

    // Return the name of this action.
    virtual std::string getName() = 0;

    // Perform the action, returning true if the player's turn should end, false
    // otherwise.
    virtual bool perform(std::string line, State* state) = 0;

   protected:
    // Regex which matches if a line is valid.
    // Must assign this in constructor of derived classes.
    std::regex matchExp;
};

// PlaceAction
// Format:
//     >place <letter> at <row letter><column number>
//     >[...]
//     >place done
class PlaceAction : public Action {
   public:
    // Constructor for PlaceAction
    PlaceAction(promptInputCallback promptInputOnly, bool* running);

    // Return the name of this action.
    std::string getName();

    // Prompt user to enter placements and construct a vector containing each,
    // until either the player has no tiles or they enter 'place done'.
    // After this, check that the set represents a valid scrabble move and
    // if so perform it, otherwise display an error message.
    bool perform(std::string line, State* state);

   private:
    // Parse line, construct a TilePlacement and add it to placements.
    // Return true if successful, falce otherwise. Does not check that the move
    // is valid.
    // Expects that line has format 'place {letter} at
    // {row_letter}{column_number}'
    bool handlePlace(std::string line, State* state,
                     std::vector<TilePlacement>& placements);

    // HACK: This reimplements Game::promptInput() so we can call it without
    // being aware of Game
    std::string promptInput();

    // Return true if placements represents a valid scrabble move, false
    // otherwise. If the move is valid, sets word to the word that will be
    // played.
    bool isValidMove(std::vector<TilePlacement> placements, State* state,
                     std::string& word);

    // Perform the move described by placements, assuming it is valid.
    void performMove(std::vector<TilePlacement> placements, State* state,
                     std::string word);

    // Check if placements contains a tile placement at the specified position.
    // If so, returns true and sets out to the placement, otherwise returns
    // false.
    bool getPlacementIfExists(std::vector<TilePlacement> placements, int rowIdx,
                              int colIdx, TilePlacement& out);

    // Return a set of TilePlacement instances that cover all the positions
    // included in the word resulting from a set of placements from the player's
    // tile bag, with order top left to bottom right.
    // Includes board tile positions preceding or following the word if they
    // exist. Does not check that there are no "holes" in the word.
    std::vector<TilePlacement> getIncludedPositions(
        State* state, std::vector<TilePlacement> placements);

    // Pointer to Game::promptInput() used to read the remainder of the
    // sequence of place commands
    promptInputCallback promptInputOnly;

    // Pointer to Game::running. The use of this is hacky, but we need it
    // to handle EOF correctly.
    bool* running;

    // Regex which matches 'place done'. Case insensitive and accepts trailing
    // whitespace and extra whitespace between words.
    std::regex placeDoneExp;

    // Scrabble dictionary
    Dictionary dictionary;
};

// ReplaceAction
// Format:
//     > replace {letter}
// Notes:
// * Remove first matching tile from player's hand and add to tile bag
// * Draw a new tile from the tile bag and add to player's hand
// * Next player's turn
class ReplaceAction : public Action {
   public:
    // Constructor
    ReplaceAction();

    // Return the name of this action.
    std::string getName();

    // Replace the first tile with specified letter in the player's hand with
    // the next tile from the tile bag.
    bool perform(std::string line, State* state);
};

// PassAction
// Format:
//     > pass
class PassAction : public Action {
   public:
    // Constructor
    PassAction();

    // Return the name of this action.
    std::string getName();

    // Do nothing and end the current player's turn. If the player passed on the
    // previous turn and the tile bag is empty, this ends the game.
    bool perform(std::string line, State* state);
};

// SaveGameAction
// Format:
//     > save {filename}
class SaveGameAction : public Action {
   public:
    // Constructor
    SaveGameAction(saveGameCallback saveGame);

    // Return the name of this action.
    std::string getName();

    // Save the game. Does not end the player's turn.
    bool perform(std::string line, State* state);

   private:
    // Pointer to Game::saveGame
    saveGameCallback saveGame;
};

// QuitAction
// Format:
//     > quit
class QuitAction : public Action {
   public:
    // Constructor
    QuitAction();

    // Return the name of this action.
    std::string getName();

    // End the game.
    bool perform(std::string line, State* state);
};

// HelpAction
// Format:
//     > quit
class HelpAction : public Action {
   public:
    // Constructor. Accepts help message to display.
    HelpAction(std::string message);

    // Return the name of this action.
    std::string getName();

    // Display the help message. Does not end turn.
    bool perform(std::string line, State* state);

   private:
    // Help message to display.
    std::string message;
};

#endif /* __ACTION_H__ */
