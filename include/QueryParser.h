
#pragma once

#include <string>
#include <vector>
#include <memory>

enum class QueryOperator {
    NONE,
    AND,
    OR
};

enum class QueryNodeType {
    TERM,
    AND,
    OR
};

struct QueryNode {
    QueryNodeType type;
    std::string term;

    std::unique_ptr<QueryNode> left;
    std::unique_ptr<QueryNode> right;

    QueryNode(QueryNodeType type, const std::string& term = "")
        : type(type), term(term) {}
};

class QueryParser {
public:
    QueryOperator getOperator(const std::string& query) const;

    std::vector<std::string> getTerms(
        const std::string& query
    ) const;

    bool isValid(const std::string& query) const;

    std::unique_ptr<QueryNode> parse(
        const std::string& query
    ) const;
};
