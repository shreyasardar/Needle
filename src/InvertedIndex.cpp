
#include "InvertedIndex.h"

#include <unordered_map>
#include <algorithm>


// Add a document to the inverted index
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


// Load previously saved index data
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


// Search for a word
const std::vector<Posting>& InvertedIndex::search(
    const std::string& word
) const {

    auto it = index.find(word);

    if (it == index.end()) {
        static const std::vector<Posting> empty;
        return empty;
    }

    return it->second;
}


// Get document length
int InvertedIndex::getDocumentLength(int documentId) const {
    return documentLengths.at(documentId);
}


// Get average document length
double InvertedIndex::getAverageDocumentLength() const {
    return averageDocumentLength;
}


// Get document frequency
int InvertedIndex::getDocumentFrequency(
    const std::string& word
) const {

    if (documentFrequency.find(word) ==
        documentFrequency.end()) {
        return 0;
    }

    return documentFrequency.at(word);
}


// Get total documents
int InvertedIndex::getTotalDocuments() const {
    return totalDocuments;
}


// Get complete index
const std::unordered_map<std::string, std::vector<Posting>>&
InvertedIndex::getIndex() const {
    return index;
}


// Get document lengths
const std::unordered_map<int, int>&
InvertedIndex::getDocumentLengths() const {
    return documentLengths;
}


// Get document frequencies
const std::unordered_map<std::string, int>&
InvertedIndex::getDocumentFrequencies() const {
    return documentFrequency;
}


// Merge two inverted indexes
void InvertedIndex::merge(const InvertedIndex& other) {

    // Merge posting lists
    for (const auto& entry : other.index) {

        const std::string& word = entry.first;

        for (const Posting& posting : entry.second) {
            index[word].push_back(posting);
        }
    }

    // Merge document lengths
    for (const auto& entry : other.documentLengths) {
        documentLengths[entry.first] = entry.second;
    }

    // Merge document frequencies
    for (const auto& entry : other.documentFrequency) {
        documentFrequency[entry.first] += entry.second;
    }

    // Update document statistics
    totalDocuments += other.totalDocuments;
    totalDocumentLength += other.totalDocumentLength;

    if (totalDocuments > 0) {

        averageDocumentLength =
            static_cast<double>(totalDocumentLength) /
            totalDocuments;
    }

    // Sort every posting list by document ID
    for (auto& entry : index) {

        std::sort(
            entry.second.begin(),
            entry.second.end(),
            [](const Posting& a, const Posting& b) {
                return a.documentId < b.documentId;
            }
        );
    }
}
