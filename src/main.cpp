#include <iostream>
#include <vector>

#include "DocumentLoader.h"
#include "Tokenizer.h"
#include "InvertedIndex.h"

int main() {

    std::cout << "Needle search engine starting...\n\n";

    DocumentLoader loader;
    std::vector<Document> documents =
        loader.load("data/corpus/documents.txt");

    Tokenizer tokenizer;

    InvertedIndex index;

    for (const Document& doc : documents) {

        std::vector<std::string> tokens =
            tokenizer.tokenize(doc.text);

        index.addDocument(doc.id, tokens);

        

        
    }

    std::vector<Posting> results =
        index.search("computer");

    std::cout << "Documents containing 'computer':\n\n";

    for (const Posting& posting : results) {

        std::cout << "Document ID: "
                  << posting.documentId << '\n';

        std::cout << "Term Frequency: "
                  << posting.termFrequency << '\n';

        std::cout << "-----------------------------\n";
    }

    return 0;
}