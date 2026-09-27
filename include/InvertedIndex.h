#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Posting.h"

class InvertedIndex {
private:
    std::unordered_map<std::string, std::vector<Posting>> index;

    std::unordered_map<int, int> documentLengths;

    double averageDocumentLength;

    int totalDocumentLength = 0;

public:
    void addDocument(
        int documentId,
        const std::vector<std::string>& tokens
    );

    std::vector<Posting> search(
        const std::string& word
    );

    int getDocumentLength(int documentId);
    double getAverageDocumentLength();
};