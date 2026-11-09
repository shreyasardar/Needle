#pragma once

#include <string>
#include "InvertedIndex.h"

class IndexReader {
public:
    bool load(
        InvertedIndex& index,
        const std::string& filename
    );
};