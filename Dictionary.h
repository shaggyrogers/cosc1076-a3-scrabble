/*
  Dictionary.h
  ============

  Description:           Represents a scrabble dictionary.
  Author:                Michael De Pasquale
  Creation Date:         2022-05-29
  Modification Date:     2022-05-29

*/

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

// Path to dictionary file.
#define DICTIONARY_FILE "wordlist.txt"

class Dictionary {
   public:
    // Constructor, loads dictionary from file.
    Dictionary(std::string file);

    // Constructor, loads DICTIONARY_FILE
    Dictionary();

    // Destructor
    ~Dictionary();

    // Returns true if the dictionary contains the specified word, false
    // otherwise. Case insensitive.
    bool contains(std::string word);

    // Returns the number of words in the dictionary
    int size();

   private:
    // Load dictionary from istream.
    void load(std::istream& stream);

    // Convert 'in' to uppercase.
    // Note this expects only letters, and may exhibit undefined behaviour for
    // strings containing non-letters.
    std::string toUpper(std::string in);

    // Use map for fast lookup of words
    std::unordered_map<std::string, bool> dict;
};

#endif /* __DICTIONARY_H__ */
