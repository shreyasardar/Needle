#include "InvertedIndex.h"

void InvertedIndex::addDocument(
    int documentId,
    const std::vector<std::string>& tokens
) {
    for (const std::string& token : tokens) {

        index[token].push_back(documentId);
    }
}

std::vector<int> InvertedIndex::search(
    const std::string& word
) {
    if (index.find(word) == index.end()) {
        return {};
    }

    return index[word];
}