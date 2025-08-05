/*
  PlayerState.h
  =============

  Description:           Represents the player state.

*/

#ifndef __PLAYERSTATE_H__
#define __PLAYERSTATE_H__

#include <assert.h>
#include <string>

#include "LinkedList.h"
#include "Tile.h"

class PlayerState {
   public:
    // Constructor
    PlayerState(std::string name, int score, LinkedList* hand);

    // Copy constructor
    PlayerState(PlayerState& state);

    // Destructor
    ~PlayerState();

    // Read player state from an istream.
    // Throws std::invalid_argument or std::out_of_range if player state data is
    // invalid.
    static PlayerState* fromStream(std::istream& stream);

    // Write player state to an ostream
    void toStream(std::ostream& stream);

    // Returns true if the specified player name is valid, false otherwise.
    // Valid names have length > 1 and consist of letters only.
    static bool isValidPlayerName(std::string name);

    // Draw tiles from tileBag until hand contains 7 tiles, or tileBag is
    // empty.
    // Returns the number of tiles drawn.
    int drawTiles(LinkedList* tileBag);

    // Attempt to remove the first tile matching the specified letter from the
    // player's hand. Returns a Tile* if successful, nullptr otherwise.
    Tile* removeTile(Letter letter);

    // Get score
    int getScore();

    // Increase score by the provided integer value
    void raiseScore(int by);

    // Get the player's name.
    std::string getName();

    // Return a comma-separated, ordered list of tiles in the player's hand,
    // suitable for display.
    std::string getHandString();

    // Return tthe number of tiles in the player's hand.
    int getNumTilesHeld();

    // Get the name of the last action performed, or an empty string if either
    // the player has not yet performed an action since starting or loading the
    // game.
    std::string getLastAction();

    // Set the name of the last action performed.
    void setLastAction(std::string action);

   private:
    // The player's name
    std::string name;

    // The player's current score
    int score;

    // The player's current hand
    LinkedList* hand;

    // The name of the last action the player performed.
    // NOTE: This is not stored when saving the game, as this would violate the
    // spec. This means that in the case where the tile bag is empty and the
    // player passes, then on their next turn, saves, quits, loads and passes
    // again, the game will not end as expected (due to passing twice with no
    // available tiles in the tile bag)
    std::string lastAction;
};

#endif /* __PLAYERSTATE_H__ */
