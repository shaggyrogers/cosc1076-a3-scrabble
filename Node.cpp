/*
  Node.cpp
  ========

  Description:           Implements a linked list node.

*/

#include "Node.h"

// Constructor
Node::Node(Tile* tile, Node* next) {
    // Tile must not be null
    assert(tile != nullptr);

    this->tile = tile;
    this->next = next;
}

// Copy constructor
Node::Node(Node& other) : Node(other.tile, other.next) {}
