#include <iostream>
#include <vector>
#include <string>

#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "BM25.h"

int main() {

    Tokenizer tokenizer;

    std::vector<std::string> tokens =
        tokenizer.tokenize("The computer networks are fast.");

    std::vector<std::string> expected = {
        "computer",
        "networks",
        "fast"
    };

    if (tokens != expected) {
        std::cout << "Tokenizer test: FAIL\n";
        return 1;
    }

    std::cout << "Tokenizer test: PASS\n";


    InvertedIndex index;

    index.addDocument(1, tokens);

    std::vector<Posting> postings =
        index.search("computer");

    if (postings.size() != 1) {
        std::cout << "Inverted index test: FAIL\n";
        return 1;
    }

    if (postings[0].documentId != 1) {
        std::cout << "Inverted index test: FAIL\n";
        return 1;
    }

    if (postings[0].termFrequency != 1) {
        std::cout << "Inverted index test: FAIL\n";
        return 1;
    }

    if (postings[0].positions.size() != 1 ||
        postings[0].positions[0] != 0) {
        std::cout << "Inverted index test: FAIL\n";
        return 1;
    }

    std::cout << "Inverted index test: PASS\n";

    BM25 bm25(index);

std::vector<SearchResult> results =
    bm25.search("computer", 1);

if (results.size() != 1) {
    std::cout << "BM25 test: FAIL\n";
    return 1;
}

if (results[0].documentId != 1) {
    std::cout << "BM25 test: FAIL\n";
    return 1;
}

if (results[0].score <= 0) {
    std::cout << "BM25 test: FAIL\n";
    return 1;
}

std::cout << "BM25 test: PASS\n";


std::vector<std::string> andTerms = {
    "computer",
    "networks"
};

std::vector<SearchResult> andResults =
    bm25.searchAND(andTerms, 2);

if (andResults.size() != 1) {
    std::cout << "AND query test: FAIL\n";
    return 1;
}

if (andResults[0].documentId != 1) {
    std::cout << "AND query test: FAIL\n";
    return 1;
}

std::cout << "AND query test: PASS\n";


std::vector<std::string> phraseWords = {
    "computer",
    "networks"
};

std::vector<SearchResult> phraseResults =
    bm25.searchPhrase(phraseWords, 2);

if (phraseResults.size() != 1) {
    std::cout << "Phrase search test: FAIL\n";
    return 1;
}

if (phraseResults[0].documentId != 1) {
    std::cout << "Phrase search test: FAIL\n";
    return 1;
}

std::cout << "Phrase search test: PASS\n";



std::vector<std::string> wrongPhrase = {
    "networks",
    "computer"
};

std::vector<SearchResult> wrongPhraseResults =
    bm25.searchPhrase(wrongPhrase, 2);

if (!wrongPhraseResults.empty()) {
    std::cout << "Negative phrase test: FAIL\n";
    return 1;
}

std::cout << "Negative phrase test: PASS\n";

    return 0;



std::vector<SearchResult> testResults = {
    {1, 5.0},
    {2, 2.0},
    {3, 9.0},
    {4, 4.0},
    {5, 7.0}
};

std::vector<SearchResult> topResults =
    bm25.getTopK(testResults, 2);

if (topResults.size() != 2) {
    std::cout << "Top-K test: FAIL\n";
    return 1;
}

if (topResults[0].documentId != 3 ||
    topResults[1].documentId != 5) {
    std::cout << "Top-K test: FAIL\n";
    return 1;
}

std::cout << "Top-K test: PASS\n";
}