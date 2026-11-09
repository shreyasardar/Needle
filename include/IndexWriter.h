
#pragma once

#include <string>
#include "InvertedIndex.h"

class IndexWriter {
public:
    bool save(
        const InvertedIndex& index,
        const std::string& filename
    );
};
