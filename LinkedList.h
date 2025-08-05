
#ifndef ASSIGN2_LINKEDLIST_H
#define ASSIGN2_LINKEDLIST_H

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Node.h"

class LinkedList {
   public:
    // Constructor
    LinkedList();

    // Copy constructor
    LinkedList(LinkedList& other);

    // Destructor
    ~LinkedList();

    // Read a string of comma-separated tiles (e.g. E-1, H-4, ...)
    static LinkedList* fromString(std::string str);

    // Create a string representation of the LinkedList instance.
    // Returns a string with a comma-separated list of tiles.
    std::string toString();

    // Add to the end of the list
    void addBack(Tile* tile);

    // Return the number of elements currently stored
    int getLength();

    // Get a Tile with the specified index
    // Raises std::out_of_range if index is invalid.
    Tile* get(int index);

    // Remove and return a copy of a Tile from the list with the specified
    // index. The returned Tile* must be managed by the caller.
    // Raises std::out_of_range if index is invalid.
    Tile* remove(int index);

    // Remove all elements from the list.
    void clear();

   private:
    int length;
    Node* head;

    // Retrieve a Node* by index
    Node* getNode(int index);
};

#endif  // ASSIGN2_LINKEDLIST_H
