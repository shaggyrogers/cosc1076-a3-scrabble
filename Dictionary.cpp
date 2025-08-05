#include "Dictionary.h"

// Constructor, loads dictionary from file.
Dictionary::Dictionary(std::string file) {
    // Open file
    std::ifstream stream(file);
    assert(stream.is_open());
    assert(stream.good());

    // Read and load dictionary
    this->load(stream);

    // Check we loaded at least one word
    assert(this->size() > 0);
}

// Constructor, loads DICTIONARY_FILE
Dictionary::Dictionary() : Dictionary(DICTIONARY_FILE) {}

// Destructor
Dictionary::~Dictionary() {}

// Returns true if the dictionary contains the specified word, false
// otherwise. Case insensitive.
bool Dictionary::contains(std::string word) {
    // Check if element exists.
    return this->dict.find(this->toUpper(word)) != this->dict.end();
}

// Returns the number of words in the dictionary
int Dictionary::size() { return this->dict.size(); }

// Load dictionary from istream.
void Dictionary::load(std::istream& stream) {
    // Load should not be called twice
    assert(this->dict.size() == 0);

    std::string curWord;
    std::getline(stream, curWord);

    while (stream.good()) {
        // Check string is not empty
        assert(curWord.length() > 0);

        // Convert to uppercase
        std::string upperWord = this->toUpper(curWord);

        // Ignore duplicates (not really necessary)
        if (!this->contains(upperWord)) {
            // Store in unordered_map
            this->dict[upperWord] = true;
        }

        // Read next word
        std::getline(stream, curWord);
    }
}

// Convert 'in' to uppercase.
// Note this expects only letters, and may exhibit undefined behaviour for
// strings containing non-letters.
std::string Dictionary::toUpper(std::string in) {
    // Convert word to upper case
    std::string upperWord;

    for (unsigned int i = 0; i < in.length(); i++) {
        upperWord.append(1, std::toupper(in[i]));
    }

    // Length should not change.
    assert(in.length() == upperWord.length());

    return upperWord;
}
