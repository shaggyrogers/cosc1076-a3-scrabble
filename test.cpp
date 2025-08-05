/*
  test.cpp
  ========

  Description:           Runs unit tests.

*/

#include <iostream>
#include <sstream>

#include "Action.h"
#include "Dictionary.h"
#include "LinkedList.h"
#include "State.h"

// Test function for LinkedList
void testLinkedList() {
    std::cout << "Testing LinkedList: ";

    LinkedList* list = new LinkedList();
    assert(list->getLength() == 0);

    // Add elements, check list is as expected
    list->addBack(new Tile('A', 1));
    assert(list->get(0)->letter == 'A' && list->get(0)->value == 1);
    assert(list->getLength() == 1);

    list->addBack(new Tile('B', 2));
    assert(list->get(1)->letter == 'B' && list->get(1)->value == 2);
    assert(list->getLength() == 2);

    // Must fail if we attempt to retrieve a non-existent element raising
    // std::out_of_range
    bool raised = false;

    try {
        list->get(3);
    } catch (std::out_of_range const&) {
        raised = true;
    }

    assert(raised);

    // Test copy constructor
    LinkedList* copy = new LinkedList(*list);
    assert(copy->toString() == list->toString());
    delete copy;

    // Test clear()
    list->clear();
    assert(list->getLength() == 0);

    // Test reading from string
    delete list;

    list = LinkedList::fromString("A-1, B-2, C-3");
    assert(list->getLength() == 3);
    assert(list->get(0)->letter == 'A' && list->get(0)->value == 1);
    assert(list->get(1)->letter == 'B' && list->get(1)->value == 2);
    assert(list->get(2)->letter == 'C' && list->get(2)->value == 3);

    // Test toString()
    assert(list->toString() == "A-1, B-2, C-3");

    // Test removing elements - Middle
    Tile* removed = list->remove(1);
    assert(removed->letter == 'B' && removed->value == 2);
    delete removed;
    assert(list->getLength() == 2);
    assert(list->get(0)->letter == 'A' && list->get(0)->value == 1);
    assert(list->get(1)->letter == 'C' && list->get(1)->value == 3);
    assert(list->toString() == "A-1, C-3");

    // Last
    removed = list->remove(1);
    assert(removed->letter == 'C' && removed->value == 3);
    delete removed;
    assert(list->getLength() == 1);
    assert(list->get(0)->letter == 'A' && list->get(0)->value == 1);
    assert(list->toString() == "A-1");

    // First/only
    removed = list->remove(0);
    assert(removed->letter == 'A' && removed->value == 1);
    assert(list->getLength() == 0);

    // Adding elements after list is empty
    list->addBack(new Tile('D', 4));
    list->addBack(new Tile('E', 1));
    assert(list->getLength() == 2);

    // Test removing first (>1 items)
    list->remove(0);
    assert(list->getLength() == 1);
    assert(list->get(0)->letter == 'E');

    // Test loading empty list from string
    delete list;
    list = LinkedList::fromString("");
    assert(list->getLength() == 0);
    delete list;

    std::cout << "OK" << std::endl;
}

// Test function for Tile
void testTile() {
    std::cout << "Testing Tile: ";

    // TODO: Should condense this using a loop

    Tile* t = Tile::fromString("A 1");
    assert(t->letter == 'A' && t->value == 1);
    delete t;

    // Leading whitespace
    t = Tile::fromString("     B 2");
    assert(t->letter == 'B' && t->value == 2);
    delete t;

    // Dash as separator
    t = Tile::fromString("D-3");
    assert(t->letter == 'D' && t->value == 3);
    delete t;

    // Dash as separator, leading whitespace
    t = Tile::fromString("     E-5");
    assert(t->letter == 'E' && t->value == 5);
    delete t;

    // Leading and trailing whitespace
    t = Tile::fromString("     B 2 ");
    assert(t->letter == 'B' && t->value == 2);
    delete t;

    // Leading and trailing whitespace, dash as separator
    t = Tile::fromString("     A-2 ");
    assert(t->letter == 'A' && t->value == 2);
    delete t;

    std::cout << "OK" << std::endl;
}

// Test function for PlayerState
void testPlayerState() {
    std::cout << "Testing PlayerState: ";

    // Test player name validation
    assert(PlayerState::isValidPlayerName("A"));
    assert(PlayerState::isValidPlayerName("Jeff"));
    assert(PlayerState::isValidPlayerName("zZ"));

    assert(!PlayerState::isValidPlayerName(""));
    assert(!PlayerState::isValidPlayerName("Player1"));
    assert(!PlayerState::isValidPlayerName("\x00\x0D\xFF"));

    // Test reading state from istream
    std::stringstream testStateRead("ABC\n14\nA-1, B-2, C-3");
    PlayerState* state = PlayerState::fromStream(testStateRead);

    assert(state->getName() == "ABC");
    assert(state->getScore() == 14);
    assert(state->getHandString() == "A-1, B-2, C-3");

    // Test writing state to ostream
    std::stringstream testStateWrite("");
    state->toStream(testStateWrite);
    assert(testStateWrite.str() == testStateRead.str());

    // Test drawing tiles
    LinkedList* tiles = State::loadScrabbleTiles(TILE_LIST_FILE);
    state->drawTiles(tiles);
    assert(state->getNumTilesHeld() == 7);

    delete tiles;
    delete state;

    std::cout << "OK" << std::endl;
}

// Test function for BoardState
void testBoardState() {
    std::cout << "Testing BoardState: ";

    // Test parseTilePosition
    int rowIdx, colIdx;
    assert(BoardState::parseTilePosition("A1", rowIdx, colIdx));
    assert(rowIdx == 0 && colIdx == 0);
    assert(BoardState::parseTilePosition("Z41", rowIdx, colIdx));
    assert(rowIdx == 25 && colIdx == 40);

    // Test reading board state from istream and various methods
    std::stringstream testStateRead("2, 2, A@A1, B@B2");
    BoardState* state = BoardState::fromStream(testStateRead);

    assert(state->getNumRows() == 2 && state->getNumCols() == 2);
    assert(state->tileExists(0, 0));
    assert(!state->tileIsAdjacentTo(0, 0));
    assert(state->getTileAt(0, 0)->letter == 'A');
    assert(state->getTileAt(0, 0)->value == 0);
    assert(!state->tileExists(0, 1) && !state->tileExists(1, 0));
    assert(state->tileIsAdjacentTo(0, 1));
    assert(state->tileIsAdjacentTo(1, 0));
    assert(state->getTileAt(0, 1) == nullptr);
    assert(state->getTileAt(1, 0) == nullptr);
    assert(state->getTileAt(1, 1)->letter == 'B');
    assert(!state->tileIsAdjacentTo(1, 1));
    assert(!state->isEmpty());
    assert(!state->isFull());

    // Test displaying board
    assert(state->draw(false) ==
           std::string("    1   2\n  ---------\nA | A |   |\nB |   | B |"));

    // Test writing bboard state to ostream
    std::stringstream testStateWrite("");
    state->toStream(testStateWrite);
    assert(testStateWrite.str() == testStateRead.str());

    delete state;

    // Test isEmpty()/isFull()
    state = new BoardState(1, 1);
    assert(state->isEmpty());
    assert(!state->isFull());

    state->placeTile(new Tile('A', 1), 0, 0);
    assert(state->isFull());

    delete state;

    std::cout << "OK" << std::endl;
}

void testState() {
    std::cout << "Testing State: ";

    std::stringstream testStateRead("");

    // Player 1 test data
    testStateRead << "PlayerA" << std::endl;
    testStateRead << "0" << std::endl;
    testStateRead << "E-1, A-1" << std::endl;

    // Player 2 test data
    testStateRead << "PlayerB" << std::endl;
    testStateRead << "5" << std::endl;
    testStateRead << "C-1, D-2" << std::endl;

    // Board state
    testStateRead << "2, 3, C@B2, D@B3" << std::endl;

    // Tile bag state
    testStateRead << "C-1, A-1, D-2, F-3" << std::endl;

    // Current player name
    testStateRead << "PlayerB";

    // Read from test stringstream
    State* state = State::fromStream(testStateRead);

    // Check player 1 data
    assert(state->getPlayer(0)->getName() == "PlayerA" &&
           state->getPlayer(0)->getScore() == 0 &&
           state->getPlayer(0)->getHandString() == "E-1, A-1");

    // Check player 2 data
    assert(state->getPlayer(1)->getName() == "PlayerB" &&
           state->getPlayer(1)->getScore() == 5 &&
           state->getPlayer(1)->getHandString() == "C-1, D-2");

    // Check board state
    assert(state->getBoard()->getNumRows() == 2 &&
           state->getBoard()->getNumCols() == 3);
    assert(state->getBoard()->getTileAt(0, 0) == nullptr);
    assert(state->getBoard()->getTileAt(0, 1) == nullptr);
    assert(state->getBoard()->getTileAt(0, 2) == nullptr);
    assert(state->getBoard()->getTileAt(1, 0) == nullptr);
    assert(state->getBoard()->getTileAt(1, 1)->letter == 'C');
    assert(state->getBoard()->getTileAt(1, 2)->letter == 'D');

    // Check tile bag state
    assert(state->getTileBag()->getLength() == 4);
    assert(state->getTileBag()->get(0)->letter == 'C' &&
           state->getTileBag()->get(0)->value == 1);
    assert(state->getTileBag()->get(1)->letter == 'A' &&
           state->getTileBag()->get(1)->value == 1);
    assert(state->getTileBag()->get(2)->letter == 'D' &&
           state->getTileBag()->get(2)->value == 2);
    assert(state->getTileBag()->get(3)->letter == 'F' &&
           state->getTileBag()->get(3)->value == 3);

    // Check current player
    assert(state->getCurrentPlayer()->getName() == "PlayerB");

    // Test writing game state to ostream
    std::stringstream testStateWrite("");
    state->toStream(testStateWrite);
    assert(testStateRead.str() == testStateWrite.str());

    delete state;

    // Test loading ScrabbleTiles.txt
    LinkedList* tiles = State::loadScrabbleTiles(TILE_LIST_FILE);
    assert(tiles->getLength() == 98);

    // Test shuffling - note this test can fail if the random order is same as
    // the initial order, but this is extremely unlikely.
    state = new State(new PlayerState("Jeff", 0, new LinkedList()),
                      new PlayerState("Frank", 0, new LinkedList()),
                      new BoardState(5, 5), new LinkedList(*tiles), 0);

    state->shuffleTileBag();
    assert(state->getTileBag()->getLength() == tiles->getLength());
    assert(state->getTileBag()->toString() != tiles->toString());

    delete tiles;
    delete state;

    std::cout << "OK" << std::endl;
}

// Tests for Action
void testAction() {
    std::cout << "Testing Action.cpp: ";

    QuitAction quit = QuitAction();

    // Test isValid()
    assert(!quit.isValid("quit abc"));
    assert(quit.isValid("quit"));

    std::cout << "OK" << std::endl;
}

// Tests for Dictionary
void testDictionary() {
    // Load dictionary
    Dictionary dict = Dictionary();

    // Check word count matches
    // 'cat wordlist.txt | tr '[:lower:]' '[:upper:]' | sort | uniq | wc -l'
    assert(dict.size() == 234371);

    // Check contains works correctly, is case insensitive
    assert(dict.contains("A"));
    assert(dict.contains("a"));
    assert(dict.contains("agrostographical"));
    assert(dict.contains("Cassegrain"));
    assert(!dict.contains("ZXXZ"));
}

// Entry point
int main(void) {
    std::cout << "Running tests" << std::endl;

    testLinkedList();
    testTile();
    testPlayerState();
    testBoardState();
    testState();
    testAction();
    testDictionary();

    std::cout << "Tests complete" << std::endl;

    return 0;
}
