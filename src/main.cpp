#include <iostream>
#include <vector>

#include "DocumentLoader.h"
#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "BM25.h"
#include "IndexWriter.h"
#include "IndexReader.h"

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

   

    IndexWriter writer;
writer.save(index, "index.txt");

InvertedIndex loadedIndex;

IndexReader reader;
reader.load(loadedIndex, "index.txt");

 BM25 bm25(loadedIndex);

    std::vector<SearchResult> results =
    bm25.search("computer",2);

    for (const SearchResult& result : results) {

    std::cout << "Document ID: "
              << result.documentId << '\n';

    std::cout << "BM25 Score: "
              << result.score << '\n';

    std::cout << "-----------------------------\n";
} 

    return 0;
}