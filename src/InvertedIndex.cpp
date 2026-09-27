#include "InvertedIndex.h"

#include <unordered_map>

void InvertedIndex::addDocument(
    int documentId,
    const std::vector<std::string>& tokens
) {

    documentLengths[documentId] = tokens.size();

    totalDocumentLength += tokens.size();
    averageDocumentLength =
    static_cast<double>(totalDocumentLength) /
    documentLengths.size();

    std::unordered_map<std::string, int> termFrequency;

    // Count how many times each word appears
    // in this document.
    for (const std::string& token : tokens) {
        termFrequency[token]++;
    }

    // Add the word and its frequency to the index.
    for (const auto& entry : termFrequency) {

        const std::string& word = entry.first;
        int frequency = entry.second;

        Posting posting;

        posting.documentId = documentId;
        posting.termFrequency = frequency;

        index[word].push_back(posting);
    }
}

std::vector<Posting> InvertedIndex::search(
    const std::string& word
) {
    if (index.find(word) == index.end()) {
        return {};
    }

    return index[word];
}

int InvertedIndex::getDocumentLength(int documentId) {
    return documentLengths[documentId];
}

double InvertedIndex::getAverageDocumentLength() {
    return averageDocumentLength;
}