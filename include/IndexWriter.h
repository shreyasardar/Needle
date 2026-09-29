#pragma once

#include <string>
#include "InvertedIndex.h"

class IndexWriter {
public:
    void save(
        const InvertedIndex& index,
        const std::string& filename
    );
};