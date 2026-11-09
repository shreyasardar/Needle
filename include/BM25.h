
#pragma once

#include <string>
#include <vector>
#include <queue>

#include "InvertedIndex.h"

struct SearchResult {
    int documentId;
    double score;
};

struct CompareSearchResult {
    bool operator()(
        const SearchResult& a,
        const SearchResult& b
    ) const {
        return a.score > b.score;
    }
};

class BM25 {
private:
    const InvertedIndex& index;

    const double k1 = 1.2;
    const double b = 0.75;

public:
    BM25(const InvertedIndex& index);

    double calculateIDF(const std::string& word);

    double calculateScore(
        double idf,
        int documentId,
        int termFrequency
    );

    // Existing search methods
    std::vector<SearchResult> search(
        const std::string& word,
        int k
    );

    std::vector<SearchResult> search(
        const std::vector<std::string>& words,
        int k
    );

    std::vector<SearchResult> searchAND(
        const std::vector<std::string>& words,
        int k
    );

    std::vector<SearchResult> searchPhrase(
        const std::vector<std::string>& words,
        int k
    );

    // New method: return all results sorted by document ID
    std::vector<SearchResult> searchByDocumentId(
        const std::string& word
    );

    std::vector<SearchResult> searchPhraseByDocumentId(
        const std::vector<std::string>& words
    );

    std::vector<SearchResult> getTopK(
        const std::vector<SearchResult>& results,
        int k
    );
};
