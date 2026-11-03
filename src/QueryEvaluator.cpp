
#include "QueryEvaluator.h"

#include <algorithm>
#include <unordered_map>
#include <string>
#include <limits>

#include "Tokenizer.h"


// Constructor
QueryEvaluator::QueryEvaluator(BM25& bm25)
    : bm25(bm25) {
}


// Evaluate the complete query tree
std::vector<SearchResult> QueryEvaluator::evaluate(
    const QueryNode* root,
    int k
) const {

    if (root == nullptr || k <= 0) {
        return {};
    }

    // Use a very large K for intermediate searches
    // so that no matching documents are discarded.
    std::vector<SearchResult> allResults =
        evaluateNode(root, std::numeric_limits<int>::max());

    // Apply Top-K only once, after evaluating the full query.
    return bm25.getTopK(allResults, k);
}


// Recursively evaluate AST nodes
std::vector<SearchResult> QueryEvaluator::evaluateNode(
    const QueryNode* node,
    int k
) const {

    if (node == nullptr) {
        return {};
    }

    // CASE 1: TERM node
    if (node->type == QueryNodeType::TERM) {

        std::string term = node->term;

        // Check whether this is a quoted phrase
        if (term.size() >= 2 &&
            term.front() == '"' &&
            term.back() == '"') {

            term = term.substr(1, term.size() - 2);

            Tokenizer tokenizer;

            std::vector<std::string> words =
                tokenizer.tokenize(term);

            if (words.empty()) {
                return {};
            }

            return bm25.searchPhrase(words, k);
        }

        // Normal term search
        Tokenizer tokenizer;

        std::vector<std::string> words =
            tokenizer.tokenize(term);

        if (words.empty()) {
            return {};
        }

        return bm25.search(words[0], k);
    }


    // Evaluate left and right subtrees
    std::vector<SearchResult> leftResults =
        evaluateNode(node->left.get(), k);

    std::vector<SearchResult> rightResults =
        evaluateNode(node->right.get(), k);


    // Store document IDs and their scores
    std::unordered_map<int, double> scores;


    // CASE 2: AND operation
  
    // CASE 2: AND operation using two-pointer intersection
    if (node->type == QueryNodeType::AND) {

        // Sort both result lists by document ID
        std::sort(
            leftResults.begin(),
            leftResults.end(),
            [](const SearchResult& a, const SearchResult& b) {
                return a.documentId < b.documentId;
            }
        );

        std::sort(
            rightResults.begin(),
            rightResults.end(),
            [](const SearchResult& a, const SearchResult& b) {
                return a.documentId < b.documentId;
            }
        );

        std::vector<SearchResult> combinedResults;

        size_t i = 0;
        size_t j = 0;

        // Find common document IDs
        while (i < leftResults.size() &&
               j < rightResults.size()) {

            if (leftResults[i].documentId ==
                rightResults[j].documentId) {

                combinedResults.push_back({
                    leftResults[i].documentId,
                    leftResults[i].score + rightResults[j].score
                });

                i++;
                j++;
            }
            else if (leftResults[i].documentId <
                     rightResults[j].documentId) {

                i++;
            }
            else {
                j++;
            }
        }

        return combinedResults;
    }



    // CASE 3: OR operation
    else if (node->type == QueryNodeType::OR) {

        // Add left-side results
        for (const auto& result : leftResults) {
            scores[result.documentId] += result.score;
        }

        // Add right-side results
        for (const auto& result : rightResults) {
            scores[result.documentId] += result.score;
        }
    }


    // Convert scores into SearchResult objects
    std::vector<SearchResult> combinedResults;

    for (const auto& entry : scores) {

        combinedResults.push_back({
            entry.first,
            entry.second
        });
    }

    // IMPORTANT:
    // Do not apply Top-K here.
    // The parent node may need documents outside the top K.

    return combinedResults;
}
