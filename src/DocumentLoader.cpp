#include "DocumentLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Document> DocumentLoader::load(const std::string& filename) {

    std::vector<Document> documents;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << '\n';
        return documents;
    }

    std::string line;

    while (std::getline(file, line)) {

        std::stringstream ss(line);

        std::string id;
        std::string title;
        std::string text;

        std::getline(ss, id, '|');
        std::getline(ss, title, '|');
        std::getline(ss, text);

        Document doc;

        doc.id = std::stoi(id);
        doc.title = title;
        doc.text = text;

        documents.push_back(doc);
    }

    return documents;
}