#include "QueryParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

QueryOperator QueryParser::getOperator(
    const std::string& query
) const {

    std::string upperQuery = query;

std::transform(
    upperQuery.begin(),
    upperQuery.end(),
    upperQuery.begin(),
    [](unsigned char c) {
        return std::toupper(c);
    }
);
    if (upperQuery.find(" AND ") != std::string::npos) {
        return QueryOperator::AND;
    }

    if (upperQuery.find(" OR ") != std::string::npos){
        return QueryOperator::OR;
    }


    return QueryOperator::NONE;
}

std::vector<std::string> QueryParser::getTerms(
    const std::string& query
) const {
    std::vector<std::string> terms;

    std::stringstream ss(query);
    std::string word;

    while (ss >> word) {
       std::string upperWord = word;

std::transform(
    upperWord.begin(),
    upperWord.end(),
    upperWord.begin(),
    [](unsigned char c) {
        return std::toupper(c);
    }
);

if (upperWord != "AND" && upperWord != "OR") {
    terms.push_back(word);
}
    }

    return terms;
}



bool QueryParser::isValid(const std::string& query) const {

    std::stringstream ss(query);
    std::vector<std::string> words;

    std::string word;

    while (ss >> word) {
        std::string upperWord = word;

        std::transform(
            upperWord.begin(),
            upperWord.end(),
            upperWord.begin(),
            [](unsigned char c) {
                return std::toupper(c);
            }
        );

        words.push_back(upperWord);
    }

    // Reject empty queries
    if (words.empty()) {
        return false;
    }

    // Reject queries starting or ending with an operator
    if (words.front() == "AND" || words.front() == "OR" ||
        words.back() == "AND" || words.back() == "OR") {
        return false;
    }

    int operatorCount = 0;
    QueryOperator detectedOperator = QueryOperator::NONE;

    for (const std::string& current : words) {

        if (current == "AND" || current == "OR") {

            operatorCount++;

            if (detectedOperator == QueryOperator::NONE) {
                detectedOperator =
                    (current == "AND")
                    ? QueryOperator::AND
                    : QueryOperator::OR;
            }

            // Reject mixed operators
            else if (
                (current == "AND" &&
                 detectedOperator != QueryOperator::AND) ||
                (current == "OR" &&
                 detectedOperator != QueryOperator::OR)
            ) {
                return false;
            }
        }
    }

    // Reject consecutive operators
    for (size_t i = 1; i < words.size(); i++) {
        bool previousIsOperator =
            words[i - 1] == "AND" || words[i - 1] == "OR";

        bool currentIsOperator =
            words[i] == "AND" || words[i] == "OR";

        if (previousIsOperator && currentIsOperator) {
            return false;
        }
    }

    // Explicit operator must have at least two operands
    if (operatorCount > 0 && words.size() < 3) {
        return false;
    }

    return true;
}
