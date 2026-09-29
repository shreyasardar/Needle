#pragma once

#include <string>
#include "InvertedIndex.h"

class IndexReader {
public:
    void load(
        InvertedIndex& index,
        const std::string& filename
    );
};