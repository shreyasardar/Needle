#pragma once

#include <string>
#include <vector>

enum class QueryOperator {
    NONE,
    AND,
    OR
};

class QueryParser {
public:
    QueryOperator getOperator(const std::string& query) const;

    std::vector<std::string> getTerms(
        const std::string& query
    ) const;
};