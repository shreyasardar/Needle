
#include "QueryEvaluator.h"

#include <algorithm>
#include <limits>
#include <string>

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

    // Keep all intermediate results.
    // Apply Top-K only after evaluating the complete query.
    std::vector<SearchResult> allResults =
        evaluateNode(root, std::numeric_limits<int>::max());

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

        Tokenizer tokenizer;

        // Check whether this is a quoted phrase
        if (term.size() >= 2 &&
            term.front() == '"' &&
            term.back() == '"') {

            term = term.substr(1, term.size() - 2);

            std::vector<std::string> words =
                tokenizer.tokenize(term);

            if (words.empty()) {
                return {};
            }

            return bm25.searchPhraseByDocumentId(words);
        }

        // Normal term search
        std::vector<std::string> words =
            tokenizer.tokenize(term);

        if (words.empty()) {
            return {};
        }

        return bm25.searchByDocumentId(words[0]);
    }


    // Evaluate left and right subtrees.
    // Both return results sorted by document ID.
    std::vector<SearchResult> leftResults =
        evaluateNode(node->left.get(), k);

    std::vector<SearchResult> rightResults =
        evaluateNode(node->right.get(), k);


    std::vector<SearchResult> combinedResults;

    size_t i = 0;
    size_t j = 0;


    // CASE 2: AND operation
    if (node->type == QueryNodeType::AND) {

        // Reserve maximum possible intersection size.
        combinedResults.reserve(
            std::min(leftResults.size(), rightResults.size())
        );

        // Two-pointer intersection.
        // Only documents present in both lists are retained.
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

        // Reserve maximum possible union size.
        combinedResults.reserve(
            leftResults.size() + rightResults.size()
        );

        // Two-pointer merge.
        // Documents from both lists are retained.
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

                combinedResults.push_back(leftResults[i]);
                i++;
            }

            else {

                combinedResults.push_back(rightResults[j]);
                j++;
            }
        }

        // Add remaining left-side results
        while (i < leftResults.size()) {
            combinedResults.push_back(leftResults[i]);
            i++;
        }

        // Add remaining right-side results
        while (j < rightResults.size()) {
            combinedResults.push_back(rightResults[j]);
            j++;
        }

        return combinedResults;
    }


    return {};
}
