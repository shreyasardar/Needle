#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Posting.h"

class InvertedIndex {
private:
    std::unordered_map<std::string, std::vector<Posting>> index;

public:
    void addDocument(
        int documentId,
        const std::vector<std::string>& tokens
    );

    std::vector<Posting> search(
        const std::string& word
    );
};