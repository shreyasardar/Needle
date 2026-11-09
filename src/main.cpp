
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <thread>
#include <functional>
#include <stdexcept>

#include "DocumentLoader.h"
#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "BM25.h"
#include "IndexWriter.h"
#include "IndexReader.h"
#include "QueryParser.h"
#include "QueryEvaluator.h"


// Build a local index for a range of documents
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
        std::cout << "  needle --build <corpus_path> <threads>\n";
        std::cout << "  needle --search <query>\n";
        std::cout << "  needle --interactive\n";
        return 1;
    }


    // =========================
    // BUILD MODE
    // =========================

    if (std::string(argv[1]) == "--build") {

        std::cout << "Build mode selected.\n";

        if (argc < 3) {
            std::cerr << "Please provide a corpus path.\n";
            return 1;
        }

        int numThreads = 1;

        try {
            if (argc >= 4) {
                numThreads = std::stoi(argv[3]);
            }
        }
        catch (const std::exception&) {
            std::cerr << "Invalid number of threads.\n";
            return 1;
        }

        if (numThreads < 1) {
            std::cerr << "Number of threads must be at least 1.\n";
            return 1;
        }


        // Start total indexing timer
        auto startTime =
            std::chrono::high_resolution_clock::now();


        // Load documents
        DocumentLoader loader;

        std::vector<Document> documents =
            loader.load(argv[2]);

        if (documents.empty()) {
            std::cerr << "No documents loaded. Check the corpus path.\n";
            return 1;
        }

        std::cout << "Documents loaded: "
                  << documents.size() << '\n';

        std::cout << "Threads used: "
                  << numThreads << '\n';


        // Create final index
        InvertedIndex index;

        // Create one local index per thread
        std::vector<InvertedIndex> localIndexes(numThreads);

        std::vector<std::thread> threads;


        // Divide documents among threads
        int documentsPerThread =
            static_cast<int>(documents.size()) / numThreads;


        // Start worker threads
        for (int i = 0; i < numThreads; i++) {

            int start = i * documentsPerThread;

            int end = start + documentsPerThread;

            // Last thread handles remaining documents
            if (i == numThreads - 1) {
                end = static_cast<int>(documents.size());
            }

            threads.emplace_back(
                buildIndexRange,
                std::cref(documents),
                start,
                end,
                std::ref(localIndexes[i])
            );
        }


        // Wait for all threads
        for (std::thread& t : threads) {
            t.join();
        }


        // Merge all local indexes
        for (int i = 0; i < numThreads; i++) {
            index.merge(localIndexes[i]);
        }


        // Save final index
        IndexWriter writer;

        writer.save(index, "index.txt");


        // Stop timer
        auto endTime =
            std::chrono::high_resolution_clock::now();

        double indexingTime =
            std::chrono::duration<double>(
                endTime - startTime
            ).count();


        // Calculate indexing throughput
        double throughput =
            index.getTotalDocuments() / indexingTime;


        // Display results
        std::cout << "\n========== INDEXING RESULTS ==========\n";

        std::cout << "Total documents indexed: "
                  << index.getTotalDocuments() << '\n';

        std::cout << "Threads used: "
                  << numThreads << '\n';

        std::cout << "Indexing time: "
                  << indexingTime << " seconds\n";

        std::cout << "Indexing throughput: "
                  << throughput << " documents/second\n";

        std::cout << "Index saved successfully.\n";

        std::cout << "======================================\n";

        return 0;
    }


    // =========================
    // START NEEDLE
    // =========================

    std::cout << "Needle search engine starting...\n\n";


    // =========================
    // INTERACTIVE MODE
    // =========================

    if (std::string(argv[1]) == "--interactive") {

        std::cout << "Interactive search mode selected.\n";

        InvertedIndex loadedIndex;

        IndexReader reader;
        reader.load(loadedIndex, "index.txt");

        BM25 bm25(loadedIndex);

        QueryParser parser;
        QueryEvaluator evaluator(bm25);

        std::string query;

        while (true) {

            std::cout << "\nQuery: ";
            std::getline(std::cin, query);

            if (query == "exit") {
                break;
            }

            // Validate query
            if (!parser.isValid(query)) {
                std::cout << "Invalid query.\n";
                continue;
            }

            // Build Abstract Syntax Tree
            auto queryTree = parser.parse(query);

            auto searchStart =
                std::chrono::high_resolution_clock::now();

            // Execute query
            std::vector<SearchResult> results =
                evaluator.evaluate(queryTree.get(), 10);

            auto searchEnd =
                std::chrono::high_resolution_clock::now();

            double searchTime =
                std::chrono::duration<double, std::milli>(
                    searchEnd - searchStart
                ).count();

            std::cout << "Search time: "
                      << searchTime
                      << " ms\n";

            if (results.empty()) {
                std::cout << "No matching documents found.\n";
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


    // =========================
    // SINGLE SEARCH MODE
    // =========================

    if (std::string(argv[1]) == "--search") {

        std::cout << "Search mode selected.\n";

        if (argc < 3) {
            std::cerr << "Please provide a search query.\n";
            return 1;
        }

        std::string query;

        for (int i = 2; i < argc; i++) {

            query += argv[i];

            if (i < argc - 1) {
                query += " ";
            }
        }

        QueryParser parser;

        // Validate query before loading index
        if (!parser.isValid(query)) {
            std::cerr << "Invalid query.\n";
            return 1;
        }

        // Build Abstract Syntax Tree
        auto queryTree = parser.parse(query);

        InvertedIndex loadedIndex;

        IndexReader reader;

        auto loadStart =
            std::chrono::high_resolution_clock::now();

        reader.load(loadedIndex, "index.txt");

        auto loadEnd =
            std::chrono::high_resolution_clock::now();

        double loadTime =
            std::chrono::duration<double, std::milli>(
                loadEnd - loadStart
            ).count();

        std::cout << "Index load time: "
                  << loadTime / 1000.0
                  << " seconds\n";

        BM25 bm25(loadedIndex);

        QueryEvaluator evaluator(bm25);

        auto searchStart =
            std::chrono::high_resolution_clock::now();

        // Execute complete query
        std::vector<SearchResult> results =
            evaluator.evaluate(queryTree.get(), 10);

        auto searchEnd =
            std::chrono::high_resolution_clock::now();

        double searchTime =
            std::chrono::duration<double, std::milli>(
                searchEnd - searchStart
            ).count();

        std::cout << "Search time: "
                  << searchTime
                  << " ms\n";

        if (results.empty()) {
            std::cout << "No matching documents found.\n";
        }

        for (const SearchResult& result : results) {

            std::cout << "Document ID: "
                      << result.documentId << '\n';

            std::cout << "BM25 Score: "
                      << result.score << '\n';

            std::cout << "-----------------------------\n";
        }

        return 0;
    }


    // =========================
    // INVALID MODE
    // =========================

    std::cout << "Unknown mode.\n";
    return 1;
}
