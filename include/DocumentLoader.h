#pragma once

#include <string>
#include <vector>

#include "Document.h"

class DocumentLoader {
public:
    std::vector<Document> load(const std::string& filename);
};