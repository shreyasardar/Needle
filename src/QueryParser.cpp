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