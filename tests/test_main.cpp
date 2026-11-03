
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "BM25.h"
#include "QueryParser.h"

int main() {

    Tokenizer tokenizer;
    QueryParser parser;

    // Test 1: Detect AND operator
    if (parser.getOperator("computer AND networks")
        != QueryOperator::AND) {
        std::cout << "QueryParser AND test: FAIL\n";
        return 1;
    }

    std::cout << "QueryParser AND test: PASS\n";

    // Test 2: Detect OR operator
    if (parser.getOperator("computer OR networks")
        != QueryOperator::OR) {
        std::cout << "QueryParser OR test: FAIL\n";
        return 1;
    }

    std::cout << "QueryParser OR test: PASS\n";

    // Test 3: Case-insensitive operator detection
    if (parser.getOperator("computer and networks")
        != QueryOperator::AND) {
        std::cout << "QueryParser lowercase test: FAIL\n";
        return 1;
    }

    std::cout << "QueryParser lowercase test: PASS\n";

    // Test 4: Extract terms without operators
    std::vector<std::string> parsedTerms =
        parser.getTerms("computer AND networks");

    std::vector<std::string> expectedTerms = {
        "computer", "networks"
    };

    if (parsedTerms != expectedTerms) {
        std::cout << "QueryParser term extraction test: FAIL\n";
        return 1;
    }

    std::cout << "QueryParser term extraction test: PASS\n";

    // Test 5: Multiple spaces
    if (parser.getOperator("computer    AND    networks")
        != QueryOperator::AND) {
        std::cout << "Multiple spaces test: FAIL\n";
        return 1;
    }

    std::cout << "Multiple spaces test: PASS\n";

    // Test 6: Empty query term extraction
    if (!parser.getTerms("").empty()) {
        std::cout << "Empty query test: FAIL\n";
        return 1;
    }

    std::cout << "Empty query test: PASS\n";

    // Test 7: Incomplete AND term extraction
    std::vector<std::string> incompleteAnd =
        parser.getTerms("computer AND");

    if (incompleteAnd.size() != 1) {
        std::cout << "Incomplete AND extraction test: FAIL\n";
        return 1;
    }

    std::cout << "Incomplete AND extraction test: PASS\n";

    // Test 8: Incomplete OR term extraction
    std::vector<std::string> incompleteOr =
        parser.getTerms("computer OR");

    if (incompleteOr.size() != 1) {
        std::cout << "Incomplete OR extraction test: FAIL\n";
        return 1;
    }

    std::cout << "Incomplete OR extraction test: PASS\n";

    // Test 9: Valid AND query
    if (!parser.isValid("computer AND networks")) {
        std::cout << "Valid AND query test: FAIL\n";
        return 1;
    }

    std::cout << "Valid AND query test: PASS\n";

    // Test 10: Valid OR query
    if (!parser.isValid("computer OR networks")) {
        std::cout << "Valid OR query test: FAIL\n";
        return 1;
    }

    std::cout << "Valid OR query test: PASS\n";

    // Test 11: Reject incomplete AND query
    if (parser.isValid("computer AND")) {
        std::cout << "Incomplete AND validation test: FAIL\n";
        return 1;
    }

    std::cout << "Incomplete AND validation test: PASS\n";

    // Test 12: Reject incomplete OR query
    if (parser.isValid("computer OR")) {
        std::cout << "Incomplete OR validation test: FAIL\n";
        return 1;
    }

    std::cout << "Incomplete OR validation test: PASS\n";

    // Test 13: Reject empty query
    if (parser.isValid("")) {
        std::cout << "Empty validation test: FAIL\n";
        return 1;
    }

    std::cout << "Empty validation test: PASS\n";

    // Test 14: Reject mixed AND and OR operators
    if (parser.isValid("computer AND networks OR database")) {
        std::cout << "Mixed operators test: FAIL\n";
        return 1;
    }

    std::cout << "Mixed operators test: PASS\n";



    // Operator at beginning
if (!parser.isValid("AND computer")) {
    std::cout << "Operator at beginning test: PASS\n";
} else {
    std::cout << "Operator at beginning test: FAIL\n";
    return 1;
}

// Consecutive operators
if (!parser.isValid("computer AND OR networks")) {
    std::cout << "Consecutive operators test: PASS\n";
} else {
    std::cout << "Consecutive operators test: FAIL\n";
    return 1;
}

// Whitespace-only query
if (!parser.isValid("   ")) {
    std::cout << "Whitespace-only query test: PASS\n";
} else {
    std::cout << "Whitespace-only query test: FAIL\n";
    return 1;
}

    // Tokenizer test
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

    // Inverted index test
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

    // BM25 test
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

    // AND query test
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

    // Phrase search test
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

    // Negative phrase search test
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

    // Index merge test
    InvertedIndex index1;
    InvertedIndex index2;

    index1.addDocument(
        1,
        {"computer", "networks"}
    );

    index2.addDocument(
        2,
        {"computer", "database"}
    );

    index1.merge(index2);

    assert(index1.getTotalDocuments() == 2);
    assert(index1.getDocumentFrequency("computer") == 2);
    assert(index1.search("computer").size() == 2);

    std::cout << "Index merge test: PASS\n";

    // Top-K test
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

    return 0;
}
