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
    const std::string& word,
    int documentId,
    int termFrequency
    );

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

std::vector<SearchResult> getTopK(
    const std::vector<SearchResult>& results,
    int k
);
};