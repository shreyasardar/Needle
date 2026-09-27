#include "BM25.h"

#include <cmath>

BM25::BM25(const InvertedIndex& index)
    : index(index) {
}

double BM25::calculateIDF(
    const std::string& word
) {
    int totalDocuments =
        index.getTotalDocuments();

    int documentFrequency =
        index.getDocumentFrequency(word);

    return std::log(
        1.0 +
        (totalDocuments - documentFrequency + 0.5) /
        (documentFrequency + 0.5)
    );
}

double BM25::calculateScore(
    const std::string& word,
    int documentId,
    int termFrequency
) {
    double idf = calculateIDF(word);

    double documentLength =
        index.getDocumentLength(documentId);

    double averageDocumentLength =
        index.getAverageDocumentLength();

    double lengthNormalization =
        1.0 -
        b +
        b * (documentLength / averageDocumentLength);

    double numerator =
        termFrequency * (k1 + 1.0);

    double denominator =
        termFrequency +
        k1 * lengthNormalization;

    return idf * (numerator / denominator);
}

std::vector<SearchResult> BM25::search(
    const std::string& word
) {
    std::vector<SearchResult> results;

    std::vector<Posting> postings =
        index.search(word);

    for (const Posting& posting : postings) {

        double score =
            calculateScore(
                word,
                posting.documentId,
                posting.termFrequency
            );

        SearchResult result;

        result.documentId = posting.documentId;
        result.score = score;

        results.push_back(result);
    }

    return results;
}