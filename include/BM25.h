#pragma once

#include <string>
#include <vector>

#include "InvertedIndex.h"

struct SearchResult {
    int documentId;
    double score;
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
};