
#include "BM25.h"
#include <unordered_map>
#include <cmath>
#include <algorithm>

// Constructor
BM25::BM25(const InvertedIndex& index)
    : index(index) {
}


// Calculate IDF
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


// Calculate BM25 score
double BM25::calculateScore(
    double idf,
    int documentId,
    int termFrequency
) {
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


// Single-word search with streaming Top-K
std::vector<SearchResult> BM25::search(
    const std::string& word, int k
) {
    if (k <= 0) {
        return {};
    }

    std::priority_queue<
        SearchResult,
        std::vector<SearchResult>,
        CompareSearchResult
    > heap;

    const std::vector<Posting>& postings =
        index.search(word);

    double idf = calculateIDF(word);

    for (const Posting& posting : postings) {

        double score = calculateScore(
            idf,
            posting.documentId,
            posting.termFrequency
        );

        SearchResult result;
        result.documentId = posting.documentId;
        result.score = score;

        heap.push(result);

        if (heap.size() > static_cast<size_t>(k)) {
            heap.pop();
        }
    }

    std::vector<SearchResult> topResults;

    while (!heap.empty()) {
        topResults.push_back(heap.top());
        heap.pop();
    }

    std::sort(
        topResults.begin(),
        topResults.end(),
        [](const SearchResult& a, const SearchResult& b) {
            return a.score > b.score;
        }
    );

    return topResults;
}


// Multi-word OR search
std::vector<SearchResult> BM25::search(
    const std::vector<std::string>& words,
    int k
) {
    if (k <= 0) {
        return {};
    }

    std::unordered_map<int, double> scores;

    for (const std::string& word : words) {

        const std::vector<Posting>& postings =
            index.search(word);

        double idf = calculateIDF(word);

        for (const Posting& posting : postings) {

            double score = calculateScore(
                idf,
                posting.documentId,
                posting.termFrequency
            );

            scores[posting.documentId] += score;
        }
    }

    std::vector<SearchResult> results;

    for (const auto& entry : scores) {
        SearchResult result;
        result.documentId = entry.first;
        result.score = entry.second;
        results.push_back(result);
    }

    return getTopK(results, k);
}


// AND search
std::vector<SearchResult> BM25::searchAND(
    const std::vector<std::string>& words,
    int k
) {
    if (words.empty() || k <= 0) {
        return {};
    }

    std::unordered_map<int, double> scores;

    const std::vector<Posting>& firstPostings =
        index.search(words[0]);

    double firstIDF = calculateIDF(words[0]);

    for (const Posting& posting : firstPostings) {

        double score = calculateScore(
            firstIDF,
            posting.documentId,
            posting.termFrequency
        );

        scores[posting.documentId] = score;
    }

    for (size_t i = 1; i < words.size(); i++) {

        const std::vector<Posting>& postings =
            index.search(words[i]);

        double idf = calculateIDF(words[i]);

        std::unordered_map<int, int> termDocuments;

        for (const Posting& posting : postings) {
            termDocuments[posting.documentId] =
                posting.termFrequency;
        }

        for (auto it = scores.begin(); it != scores.end();) {

            int documentId = it->first;

            if (termDocuments.find(documentId) ==
                termDocuments.end()) {

                it = scores.erase(it);

            } else {

                int termFrequency =
                    termDocuments[documentId];

                it->second += calculateScore(
                    idf,
                    documentId,
                    termFrequency
                );

                ++it;
            }
        }
    }

    std::vector<SearchResult> results;

    for (const auto& entry : scores) {
        SearchResult result;
        result.documentId = entry.first;
        result.score = entry.second;
        results.push_back(result);
    }

    return getTopK(results, k);
}


// Phrase search
std::vector<SearchResult> BM25::searchPhrase(
    const std::vector<std::string>& words,
    int k
) {
    std::vector<SearchResult> results;

    if (words.empty() || k <= 0) {
        return results;
    }

    const std::vector<Posting>& firstPostings =
        index.search(words[0]);

    for (const Posting& firstPosting : firstPostings) {

        int documentId = firstPosting.documentId;

        bool phraseFound = false;

        for (int startPosition : firstPosting.positions) {

            bool matches = true;

            for (size_t i = 1; i < words.size(); i++) {

                const std::vector<Posting>& postings =
                    index.search(words[i]);

                bool foundPosition = false;

                for (const Posting& posting : postings) {

                    if (posting.documentId != documentId) {
                        continue;
                    }

                    int requiredPosition =
                        startPosition + static_cast<int>(i);

                    for (int position : posting.positions) {

                        if (position == requiredPosition) {
                            foundPosition = true;
                            break;
                        }
                    }

                    break;
                }

                if (!foundPosition) {
                    matches = false;
                    break;
                }
            }

            if (matches) {
                phraseFound = true;
                break;
            }
        }

        if (phraseFound) {

            double score = 0.0;

            for (const std::string& word : words) {

                const std::vector<Posting>& postings =
                    index.search(word);

                double idf = calculateIDF(word);

                for (const Posting& posting : postings) {

                    if (posting.documentId == documentId) {

                        score += calculateScore(
                            idf,
                            documentId,
                            posting.termFrequency
                        );

                        break;
                    }
                }
            }

            SearchResult result;
            result.documentId = documentId;
            result.score = score;

            results.push_back(result);
        }
    }

    return getTopK(results, k);
}


// Get the best K results
std::vector<SearchResult> BM25::getTopK(
    const std::vector<SearchResult>& results,
    int k
) {
    if (k <= 0) {
        return {};
    }

    std::priority_queue<
        SearchResult,
        std::vector<SearchResult>,
        CompareSearchResult
    > heap;

    for (const SearchResult& result : results) {

        heap.push(result);

        if (heap.size() > static_cast<size_t>(k)) {
            heap.pop();
        }
    }

    std::vector<SearchResult> topResults;

    while (!heap.empty()) {
        topResults.push_back(heap.top());
        heap.pop();
    }

    std::sort(
        topResults.begin(),
        topResults.end(),
        [](const SearchResult& a, const SearchResult& b) {
            return a.score > b.score;
        }
    );

    return topResults;
}
