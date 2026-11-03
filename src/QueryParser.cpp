
#include "QueryParser.h"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <memory>


// ========================================
// Convert string to uppercase
// ========================================

std::string toUpper(const std::string& input) {

    std::string result = input;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char c) {
            return std::toupper(c);
        }
    );

    return result;
}


// ========================================
// Tokenize the query
// ========================================

bool tokenizeQuery(
    const std::string& query,
    std::vector<std::string>& tokens
) {

    size_t i = 0;

    while (i < query.size()) {

        // Skip whitespace
        if (std::isspace(static_cast<unsigned char>(query[i]))) {
            i++;
            continue;
        }

        // Recognize parentheses
        if (query[i] == '(' || query[i] == ')') {

            tokens.push_back(std::string(1, query[i]));

            i++;
            continue;
        }

        // Recognize quoted phrases
        if (query[i] == '"') {

            size_t start = i;

            i++;

            while (i < query.size() && query[i] != '"') {
                i++;
            }

            // Reject unmatched quotation marks
            if (i == query.size()) {
                return false;
            }

            i++;

            tokens.push_back(
                query.substr(start, i - start)
            );

            continue;
        }

        // Read normal terms
        size_t start = i;

        while (
            i < query.size() &&
            !std::isspace(static_cast<unsigned char>(query[i])) &&
            query[i] != '(' &&
            query[i] != ')'
        ) {
            i++;
        }

        tokens.push_back(
            query.substr(start, i - start)
        );
    }

    return true;
}


// ========================================
// Recursive Descent Expression Parser
// ========================================

class ExpressionParser {

private:

    const std::vector<std::string>& tokens;

    size_t position = 0;


public:

    ExpressionParser(
        const std::vector<std::string>& tokens
    ) : tokens(tokens) {}


    // Entry point
    std::unique_ptr<QueryNode> parseExpression() {

        return parseOR();
    }


private:

    // ====================================
    // OR has lower precedence
    // ====================================

    std::unique_ptr<QueryNode> parseOR() {

        auto left = parseAND();

        if (!left) {
            return nullptr;
        }

        while (
            position < tokens.size() &&
            toUpper(tokens[position]) == "OR"
        ) {

            position++;

            auto right = parseAND();

            if (!right) {
                return nullptr;
            }

            auto parent = std::make_unique<QueryNode>(
                QueryNodeType::OR
            );

            parent->left = std::move(left);
            parent->right = std::move(right);

            left = std::move(parent);
        }

        return left;
    }


    // ====================================
    // AND has higher precedence
    // Also supports implicit AND
    // ====================================

    std::unique_ptr<QueryNode> parseAND() {

        auto left = parsePrimary();

        if (!left) {
            return nullptr;
        }

        while (position < tokens.size()) {

            std::string current = toUpper(tokens[position]);

            // Stop when OR is encountered
            if (current == "OR" || tokens[position] == ")") {
                break;
            }

            // Explicit AND
            if (current == "AND") {
                position++;
            }

            // Otherwise, adjacent terms mean implicit AND

            auto right = parsePrimary();

            if (!right) {
                return nullptr;
            }

            auto parent = std::make_unique<QueryNode>(
                QueryNodeType::AND
            );

            parent->left = std::move(left);
            parent->right = std::move(right);

            left = std::move(parent);
        }

        return left;
    }


    // ====================================
    // Handle terms and parentheses
    // ====================================

    std::unique_ptr<QueryNode> parsePrimary() {

        if (position >= tokens.size()) {
            return nullptr;
        }

        std::string current = tokens[position];

        // Handle parenthesized expression
        if (current == "(") {

            position++;

            auto node = parseOR();

            // Closing parenthesis must exist
            if (
                !node ||
                position >= tokens.size() ||
                tokens[position] != ")"
            ) {
                return nullptr;
            }

            position++;

            return node;
        }

        std::string upper = toUpper(current);

        // Operators cannot be standalone terms
        if (
            upper == "AND" ||
            upper == "OR" ||
            current == ")"
        ) {
            return nullptr;
        }

        position++;

        return std::make_unique<QueryNode>(
            QueryNodeType::TERM,
            current
        );
    }


public:

    // Check whether all tokens were consumed
    bool isFullyParsed() const {

        return position == tokens.size();
    }
};


// ========================================
// Get first detected Boolean operator
// ========================================

QueryOperator QueryParser::getOperator(
    const std::string& query
) const {

    std::vector<std::string> tokens;

    if (!tokenizeQuery(query, tokens)) {
        return QueryOperator::NONE;
    }

    for (const std::string& token : tokens) {

        std::string upper = toUpper(token);

        if (upper == "AND") {
            return QueryOperator::AND;
        }

        if (upper == "OR") {
            return QueryOperator::OR;
        }
    }

    return QueryOperator::NONE;
}


// ========================================
// Extract query terms
// ========================================

std::vector<std::string> QueryParser::getTerms(
    const std::string& query
) const {

    std::vector<std::string> tokens;
    std::vector<std::string> terms;

    if (!tokenizeQuery(query, tokens)) {
        return terms;
    }

    for (const std::string& token : tokens) {

        std::string upper = toUpper(token);

        if (
            upper != "AND" &&
            upper != "OR" &&
            token != "(" &&
            token != ")"
        ) {

            terms.push_back(token);
        }
    }

    return terms;
}


// ========================================
// Build Abstract Syntax Tree
// ========================================

std::unique_ptr<QueryNode> QueryParser::parse(
    const std::string& query
) const {

    std::vector<std::string> tokens;

    if (!tokenizeQuery(query, tokens) || tokens.empty()) {
        return nullptr;
    }

    ExpressionParser parser(tokens);

    auto root = parser.parseExpression();

    if (!root) {
        return nullptr;
    }

    if (!parser.isFullyParsed()) {
        return nullptr;
    }

    return root;
}


// ========================================
// Validate query
// ========================================

bool QueryParser::isValid(
    const std::string& query
) const {

    auto root = parse(query);

    return root != nullptr;
}
