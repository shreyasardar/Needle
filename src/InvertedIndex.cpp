#include "InvertedIndex.h"

#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <utility>
#include <limits>


// Add a document to the inverted index
bool InvertedIndex::addDocument(
    int documentId,
    const std::vector<std::string>& tokens
) {

    // Reject negative document IDs.
    if (documentId < 0) {
        return false;
    }

    // Reject duplicate document IDs.
    if (documentLengths.find(documentId) != documentLengths.end()) {
        return false;
    }

    // Prevent document length overflow.
    if (tokens.size() >
        static_cast<size_t>(std::numeric_limits<int>::max())) {
        return false;
    }

    // Prevent total document count overflow.
    if (totalDocuments == std::numeric_limits<int>::max()) {
        return false;
    }

    int documentLength = static_cast<int>(tokens.size());

    // Prevent total document length overflow.
    if (documentLength >
        std::numeric_limits<int>::max() - totalDocumentLength) {
        return false;
    }

    // Reject empty terms.
    for (const std::string& token : tokens) {
        if (token.empty()) {
            return false;
        }
    }

    // Calculate term frequencies and positions.
    std::unordered_map<std::string, int> termFrequency;
    std::unordered_map<std::string, std::vector<int>> positions;

    for (int i = 0; i < documentLength; i++) {

        const std::string& token = tokens[i];

        termFrequency[token]++;
        positions[token].push_back(i);
    }

    // Update document metadata.
    documentLengths[documentId] = documentLength;

    totalDocumentLength += documentLength;

    totalDocuments++;

    averageDocumentLength =
        static_cast<double>(totalDocumentLength) /
        totalDocuments;

    // Update inverted index and document frequencies.
    for (const auto& entry : termFrequency) {

        const std::string& word = entry.first;
        int frequency = entry.second;

        documentFrequency[word]++;

        Posting posting;
        posting.documentId = documentId;
        posting.termFrequency = frequency;
        posting.positions = std::move(positions[word]);
index[word].push_back(std::move(posting));
    }

    return true;
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

    totalDocumentLength = 0;

    for (const auto& entry : documentLengths) {
        totalDocumentLength += entry.second;
    }
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

    auto it = documentFrequency.find(word);

    if (it == documentFrequency.end()) {
        return 0;
    }

    return it->second;
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
// Merge two inverted indexes
void InvertedIndex::merge(InvertedIndex& other) {

    // Reserve capacity to reduce hash-map rehashing.
    index.reserve(index.size() + other.index.size());

    // Merge posting lists.
    for (auto& entry : other.index) {

        const std::string& word = entry.first;
        std::vector<Posting>& otherPostings = entry.second;

        auto it = index.find(word);

        // If the word does not exist, transfer its posting list.
        if (it == index.end()) {
            index.emplace(word, std::move(otherPostings));
        }

        else {

            std::vector<Posting>& currentPostings = it->second;

            std::vector<Posting> mergedPostings;

            mergedPostings.reserve(
                currentPostings.size() + otherPostings.size()
            );

            // Merge by moving postings instead of copying them.
            std::merge(
                std::make_move_iterator(currentPostings.begin()),
                std::make_move_iterator(currentPostings.end()),
                std::make_move_iterator(otherPostings.begin()),
                std::make_move_iterator(otherPostings.end()),
                std::back_inserter(mergedPostings),
                [](const Posting& a, const Posting& b) {
                    return a.documentId < b.documentId;
                }
            );

            it->second = std::move(mergedPostings);
        }
    }

    // Reserve capacity for document metadata.
    documentLengths.reserve(
        documentLengths.size() + other.documentLengths.size()
    );

    documentFrequency.reserve(
        documentFrequency.size() + other.documentFrequency.size()
    );

    // Merge document lengths.
    for (const auto& entry : other.documentLengths) {
        documentLengths[entry.first] = entry.second;
    }

    // Merge document frequencies.
    for (const auto& entry : other.documentFrequency) {
        documentFrequency[entry.first] += entry.second;
    }

    // Update document statistics.
    totalDocuments += other.totalDocuments;
    totalDocumentLength += other.totalDocumentLength;

    if (totalDocuments > 0) {
        averageDocumentLength =
            static_cast<double>(totalDocumentLength) /
            totalDocuments;
    }
}