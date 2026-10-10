#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <thread>
#include <functional>

#include "DocumentLoader.h"
#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "BM25.h"
#include "IndexWriter.h"
#include "IndexReader.h"
#include "QueryParser.h"


void buildIndexRange(
    const std::vector<Document>& documents,
    int start,
    int end,
    InvertedIndex& localIndex
) {
    Tokenizer tokenizer;

    for (int i = start; i < end; i++) {
        std::vector<std::string> tokens =
            tokenizer.tokenize(documents[i].text);

        localIndex.addDocument(
            documents[i].id,
            tokens
        );
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
    std::cout << "Usage:\n";
    std::cout << "  needle --build <corpus_path>\n";
    std::cout << "  needle --search <query>\n";
    std::cout << "  needle --interactive\n";
    return 1;
}

if (std::string(argv[1]) == "--build") {
    std::cout << "Build mode selected.\n";

    if (argc < 3) {
        std::cout << "Please provide a corpus path.\n";
        return 1;
    }

    int numThreads = 1;

if (argc >= 4) {
    numThreads = std::stoi(argv[3]);
}

if (numThreads < 1) {
    std::cout << "Number of threads must be at least 1.\n";
    return 1;
}

    DocumentLoader loader;
    std::vector<Document> documents =
        loader.load(argv[2]);

    Tokenizer tokenizer;


   InvertedIndex index;

   std::vector<InvertedIndex> localIndexes(numThreads);

std::vector<std::thread> threads;

int documentsPerThread =
    documents.size() / numThreads;

    for (int i = 0; i < numThreads; i++) {
    int start = i * documentsPerThread;
    int end = start + documentsPerThread;

    if (i == numThreads - 1) {
        end = documents.size();
    }

    threads.emplace_back(
        buildIndexRange,
        std::cref(documents),
        start,
        end,
        std::ref(localIndexes[i])
    );
}

for (std::thread& t : threads) {
    t.join();
}

for (int i = 0; i < numThreads; i++) {
    index.merge(localIndexes[i]);
}


    IndexWriter writer;
writer.save(index, "index.txt");
}

    std::cout << "Needle search engine starting...\n\n";

  if (std::string(argv[1]) == "--interactive") {
    std::cout << "Interactive search mode selected.\n";

    InvertedIndex loadedIndex;

    IndexReader reader;
    reader.load(loadedIndex, "index.txt");

    BM25 bm25(loadedIndex);

    Tokenizer tokenizer;
QueryParser parser;

    std::string query;

while (true) {
    std::cout << "\nQuery: ";
    std::getline(std::cin, query);

    if (query == "exit") {
        break;
    }

    QueryOperator operation =
    parser.getOperator(query);

bool isPhraseQuery =
    query.size() >= 2 &&
    query.front() == '"' &&
    query.back() == '"';

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



auto searchStart = std::chrono::high_resolution_clock::now();

std::vector<SearchResult> results;

if (isPhraseQuery) {
    results = bm25.searchPhrase(queryTokens, 2);
} else if (operation == QueryOperator::AND) {
    results = bm25.searchAND(queryTokens, 2);
} else {
    results = bm25.search(queryTokens, 2);
}

auto searchEnd = std::chrono::high_resolution_clock::now();

double searchTime =
    std::chrono::duration<double, std::milli>(
        searchEnd - searchStart
    ).count();

std::cout << "Search time: "
          << searchTime
          << " ms\n";
for (const SearchResult& result : results) {
    std::cout << "Document ID: "
              << result.documentId << '\n';

    std::cout << "BM25 Score: "
              << result.score << '\n';

    std::cout << "-----------------------------\n";
}
}
}

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

auto loadStart = std::chrono::high_resolution_clock::now();

reader.load(loadedIndex, "index.txt");

auto loadEnd = std::chrono::high_resolution_clock::now();

double loadTime =
    std::chrono::duration<double>(loadEnd - loadStart).count();

std::cout << "Index load time: "
          << loadTime
          << " seconds\n";

BM25 bm25(loadedIndex);

Tokenizer tokenizer;
QueryParser parser;

QueryOperator operation =
    parser.getOperator(query);

    bool isPhraseQuery =
    query.size() >= 2 &&
    query.front() == '"' &&
    query.back() == '"';

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

if (isPhraseQuery) {
    results = bm25.searchPhrase(queryTokens, 2);
} else if (operation == QueryOperator::AND) {
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