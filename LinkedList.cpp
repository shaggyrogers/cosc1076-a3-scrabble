
#include "LinkedList.h"

// Constructor
LinkedList::LinkedList() {
    this->head = nullptr;
    this->length = 0;
}

// Copy constructor
LinkedList::LinkedList(LinkedList& other) : LinkedList() {
    // Add copies of each Node
    for (int i = 0; i < other.getLength(); i++) {
        this->addBack(other.get(i));
    }

    // Length should be the same
    assert(this->getLength() == other.getLength());
}

// Destructor
LinkedList::~LinkedList() { this->clear(); }

// Read a string of comma-separated tiles in the format found in game save
// files (e.g. E-1, H-4, ...) and return a LinkedList*.
// TODO: Change to std::string?
LinkedList* LinkedList::fromString(std::string str) {
    LinkedList* result = new LinkedList();

    // Do not attempt to parse empty string (will fail)
    if (str != "") {
        // Create stringstream and per-element buffer for processing
        std::stringstream stream(str);
        char buf[1024];

        // Read/parse each element and add to the LinkedList
        while (stream.good()) {
            stream.getline(buf, sizeof(buf) - 1, ',');
            result->addBack(Tile::fromString(buf));
        }
    }

    return result;
}

// Create a string representation of the LinkedList instance.
// Returns a string with a comma-separated list of tiles.
std::string LinkedList::toString() {
    std::stringstream stream;

    // Write first element to stream, if it exists
    Node* node = this->head;

    if (node != nullptr) {
        assert(node->tile != nullptr);
        stream << node->tile->letter << '-' << node->tile->value;
        node = node->next;
    }

    // Write remaining elements to stream, if they exist
    while (node != nullptr) {
        assert(node->tile != nullptr);
        stream << ", " << node->tile->letter << '-' << node->tile->value;
        node = node->next;
    }

    // Return copy of stream contents as std::string
    return stream.str();
}

// Add to the end of the list
void LinkedList::addBack(Tile* tile) {
    assert(tile != nullptr);

    // Create node
    Node* newNode = new Node(tile, nullptr);

    if (this->head == nullptr) {
        // Special case - list is empty
        assert(this->length == 0);
        this->head = newNode;
    } else {
        // Add to end of the linked list
        Node* oldTail = this->getNode(this->length - 1);
        assert(oldTail->next == nullptr);
        oldTail->next = newNode;
    }

    this->length++;
}

// Return the number of elements currently stored
int LinkedList::getLength() { return this->length; }

// Get a Tile with the specified index
// Raises std::out_of_range if index is invalid.
Tile* LinkedList::get(int index) { return this->getNode(index)->tile; }

// Remove and return a copy of a Tile from the list with the specified index.
// The returned Tile* must be managed by the caller.
// Raises std::out_of_range if index is invalid.
Tile* LinkedList::remove(int index) {
    // Get target node
    Node* removed = this->getNode(index);

    // Update linked list to remove target node
    if (index == 0) {
        // First element - just change head
        this->head = this->length > 1 ? this->getNode(1) : nullptr;
    } else if (index > 0 && index < this->getLength()) {
        // All other elements - change previous->next to element->next
        Node* prev = getNode(index - 1);
        prev->next = removed->next;
    }

    // Update length
    this->length--;
    assert(this->length >= 0);

    // Copy Tile before deleting node
    Tile* result = new Tile(*removed->tile);
    delete removed;

    return result;
}

// Retrieve a Node* by index
Node* LinkedList::getNode(int index) {
    // Throw exception if index is not valid
    if (index < 0 || index >= this->length) {
        throw std::out_of_range("Index out of range");
    }

    // Traverse list until we reach the element at the specified index
    Node* cur = this->head;

    for (int i = 0; i != index; i++) {
        cur = cur->next;
    }

    return cur;
}

// Remove all elements from the list.
void LinkedList::clear() {
    while (this->getLength() > 0) {
        delete this->remove(0);
    }
}
