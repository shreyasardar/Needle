
#pragma once

#include <vector>

#include "QueryParser.h"
#include "BM25.h"

class QueryEvaluator {
private:
     BM25& bm25;

    std::vector<SearchResult> evaluateNode(
        const QueryNode* node,
        int k
    ) const;

public:
    QueryEvaluator( BM25& bm25);

    std::vector<SearchResult> evaluate(
        const QueryNode* root,
        int k
    ) const;
};
