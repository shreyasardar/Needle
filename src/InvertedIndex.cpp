#include "InvertedIndex.h"

#include <unordered_map>

void InvertedIndex::addDocument(
    int documentId,
    const std::vector<std::string>& tokens
) {

    documentLengths[documentId] = tokens.size();

    totalDocumentLength += tokens.size();

    totalDocuments++;


    averageDocumentLength =
    static_cast<double>(totalDocumentLength) /
    documentLengths.size();

  std::unordered_map<std::string, int> termFrequency;
std::unordered_map<std::string, std::vector<int>> positions;

for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
    const std::string& token = tokens[i];

    termFrequency[token]++;
    positions[token].push_back(i);
}

for (const auto& entry : termFrequency) {
    const std::string& word = entry.first;
    int frequency = entry.second;

    documentFrequency[word]++;

    Posting posting;
    posting.documentId = documentId;
    posting.termFrequency = frequency;
    posting.positions = positions[word];

    index[word].push_back(posting);
}
}

void InvertedIndex::loadData(
    const std::unordered_map<std::string, std::vector<Posting>>& loadedIndex,
    const std::unordered_map<int, int>& loadedDocumentLengths,
    const std::unordered_map<std::string, int>& loadedDocumentFrequencies,
    int loadedTotalDocuments,
    double loadedAverageDocumentLength
) {
    index = loadedIndex;
    documentLengths = loadedDocumentLengths;
    documentFrequency = loadedDocumentFrequencies;
    totalDocuments = loadedTotalDocuments;
    averageDocumentLength = loadedAverageDocumentLength;
}

std::vector<Posting> InvertedIndex::search(
    const std::string& word
) const {
    if (index.find(word) == index.end()) {
        return {};
    }

    return index.at(word);
}

int InvertedIndex::getDocumentLength(int documentId) const {
    return documentLengths.at(documentId);
}

double InvertedIndex::getAverageDocumentLength()const {
    return averageDocumentLength;
}

int InvertedIndex::getDocumentFrequency(
    const std::string& word
) const {
    if (documentFrequency.find(word) ==
        documentFrequency.end()) {
        return 0;
    }

    return documentFrequency.at(word);
}

int InvertedIndex::getTotalDocuments() const {
    return totalDocuments;
}

const std::unordered_map<std::string, std::vector<Posting>>&
InvertedIndex::getIndex() const {
    return index;
}

const std::unordered_map<int, int>&
InvertedIndex::getDocumentLengths() const {
    return documentLengths;
}

const std::unordered_map<std::string, int>&
InvertedIndex::getDocumentFrequencies() const {
    return documentFrequency;
}

void InvertedIndex::merge(const InvertedIndex& other) {
    for (const auto& entry : other.index) {
        const std::string& word = entry.first;

        for (const Posting& posting : entry.second) {
            index[word].push_back(posting);
        }
    }

    for (const auto& entry : other.documentLengths) {
        documentLengths[entry.first] = entry.second;
    }

    for (const auto& entry : other.documentFrequency) {
        documentFrequency[entry.first] += entry.second;
    }

    totalDocuments += other.totalDocuments;
    totalDocumentLength += other.totalDocumentLength;

    if (totalDocuments > 0) {
        averageDocumentLength =
            static_cast<double>(totalDocumentLength) /
            totalDocuments;
    }
}