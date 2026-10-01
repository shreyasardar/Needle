#pragma once

#include <vector>

struct Posting {
    int documentId;
    int termFrequency;
    std::vector<int> positions;
};