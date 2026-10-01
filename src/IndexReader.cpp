#include "IndexReader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

void IndexReader::load(
    InvertedIndex& index,
    const std::string& filename
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open index file: "
                  << filename << '\n';
        return;
    }
       int totalDocuments = 0;
       double averageDocumentLength = 0.0;
       std::unordered_map<int, int> documentLengths;
       std::unordered_map<std::string, int> documentFrequencies;
       std::unordered_map<std::string, std::vector<Posting>> loadedIndex;

        std::string line;
    std::string section = "INDEX";

    while (std::getline(file, line)) {

        if (line == "DOCUMENT_LENGTHS") {
            section = "DOCUMENT_LENGTHS";
            continue;
        }

        if (line == "DOCUMENT_FREQUENCIES") {
            section = "DOCUMENT_FREQUENCIES";
            continue;
        }

        if (line == "TOTAL_DOCUMENTS") {
            section = "TOTAL_DOCUMENTS";
            continue;
        }

        if (line == "AVERAGE_DOCUMENT_LENGTH") {
            section = "AVERAGE_DOCUMENT_LENGTH";
            continue;
        }

        if (line.empty()) {
            continue;
        }

        // We will parse each section here.

        if (section == "TOTAL_DOCUMENTS") {
    std::stringstream ss(line);
    ss >> totalDocuments;
    continue;
}


if (section == "AVERAGE_DOCUMENT_LENGTH") {
    std::stringstream ss(line);
    ss >> averageDocumentLength;
    continue;
}

if (section == "DOCUMENT_LENGTHS") {
    std::stringstream ss(line);

    int documentId;
    int documentLength;

    ss >> documentId >> documentLength;

    documentLengths[documentId] = documentLength;

    continue;
}

if (section == "DOCUMENT_FREQUENCIES") {
    std::stringstream ss(line);

    std::string word;
    int frequency;

    ss >> word >> frequency;

    documentFrequencies[word] = frequency;

    continue;
}

if (section == "INDEX") {
    std::stringstream ss(line);

    std::string word;
    ss >> word;

    std::string postingData;

   while (ss >> postingData) {

    size_t firstColon =
        postingData.find(':');

    size_t secondColon =
        postingData.find(':', firstColon + 1);

    int documentId =
        std::stoi(
            postingData.substr(
                0,
                firstColon
            )
        );

    int termFrequency =
        std::stoi(
            postingData.substr(
                firstColon + 1,
                secondColon - firstColon - 1
            )
        );

    Posting posting;
    posting.documentId = documentId;
    posting.termFrequency = termFrequency;

    std::string positionData =
        postingData.substr(secondColon + 1);

    std::stringstream positionStream(
        positionData
    );

    std::string position;

    while (std::getline(
        positionStream,
        position,
        ','
    )) {
        if (!position.empty()) {
            posting.positions.push_back(
                std::stoi(position)
            );
        }
    }

    loadedIndex[word].push_back(posting);
}

    continue;
}
    }
    index.loadData(
    loadedIndex,
    documentLengths,
    documentFrequencies,
    totalDocuments,
    averageDocumentLength
);
}