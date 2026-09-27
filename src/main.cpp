#include <iostream>
#include <vector>

#include "DocumentLoader.h"
#include "Tokenizer.h"
#include "InvertedIndex.h"

int main() {

    std::cout << "Needle search engine starting...\n\n";

    // Load documents
    DocumentLoader loader;

    std::vector<Document> documents =
        loader.load("data/corpus/documents.txt");

    // Create tokenizer
    Tokenizer tokenizer;

    // Create inverted index
    InvertedIndex index;

    // Process every document
    for (const Document& doc : documents) {

        std::vector<std::string> tokens =
            tokenizer.tokenize(doc.text);

        index.addDocument(doc.id, tokens);
    }

    // Search the index
    std::vector<int> results =
        index.search("systems");

    std::cout << "Documents containing 'computer':\n";

    for (int documentId : results) {
        std::cout << documentId << '\n';
    }

    return 0;
}