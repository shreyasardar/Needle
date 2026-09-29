#include <iostream>
#include <vector>

#include "DocumentLoader.h"
#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "BM25.h"
#include "IndexWriter.h"
#include "IndexReader.h"
#include "QueryParser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
    std::cout << "Usage:\n";
    std::cout << "  needle --build\n";
    std::cout << "  needle --search <query>\n";
    return 1;
}

if (std::string(argv[1]) == "--build") {
    std::cout << "Build mode selected.\n";

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
}

    std::cout << "Needle search engine starting...\n\n";

  if (std::string(argv[1]) == "--search") {
    std::cout << "Search mode selected.\n";

if (argc < 3) {
    std::cout << "Please provide a search query.\n";
    return 1;
}

std::string query;

for (int i = 2; i < argc; i++) {
    query += argv[i];

    if (i < argc - 1) {
        query += " ";
    }
}
InvertedIndex loadedIndex;

IndexReader reader;
reader.load(loadedIndex, "index.txt");

BM25 bm25(loadedIndex);

Tokenizer tokenizer;
QueryParser parser;

QueryOperator operation =
    parser.getOperator(query);

std::vector<std::string> queryTerms =
    parser.getTerms(query);

std::vector<std::string> queryTokens;

for (const std::string& term : queryTerms) {
    std::vector<std::string> tokens =
        tokenizer.tokenize(term);

    for (const std::string& token : tokens) {
        queryTokens.push_back(token);
    }
}

std::vector<SearchResult> results;

if (operation == QueryOperator::AND) {
    results = bm25.searchAND(queryTokens, 2);
} else {
    results = bm25.search(queryTokens, 2);
}

    for (const SearchResult& result : results) {
    std::cout << "Document ID: "
              << result.documentId << '\n';

    std::cout << "BM25 Score: "
              << result.score << '\n';

    std::cout << "-----------------------------\n";
}
}

   

    





    return 0;
}