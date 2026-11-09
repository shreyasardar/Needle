#include "IndexReader.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Converts a string to a non-negative integer safely.
bool parseNonNegativeInt(const std::string& text, int& value) {
    if (text.empty()) {
        return false;
    }

    try {
        size_t parsed = 0;
        long long number = std::stoll(text, &parsed);

        if (parsed != text.size() ||
            number < 0 ||
            number > std::numeric_limits<int>::max()) {
            return false;
        }

        value = static_cast<int>(number);
        return true;
    } catch (...) {
        return false;
    }
}

// Parses one posting in the format: documentId:termFrequency:positions
bool parsePosting(const std::string& token, Posting& posting) {
    const size_t firstColon = token.find(':');

    if (firstColon == std::string::npos) {
        return false;
    }

    const size_t secondColon = token.find(':', firstColon + 1);

    if (secondColon == std::string::npos ||
        token.find(':', secondColon + 1) != std::string::npos) {
        return false;
    }

    if (!parseNonNegativeInt(
            token.substr(0, firstColon),
            posting.documentId) ||
        !parseNonNegativeInt(
            token.substr(firstColon + 1,
                         secondColon - firstColon - 1),
            posting.termFrequency)) {
        return false;
    }

    if (posting.termFrequency <= 0) {
        return false;
    }

    const std::string positionsText = token.substr(secondColon + 1);

    if (positionsText.empty() || positionsText.back() != ',') {
        return false;
    }

    std::stringstream positionsStream(positionsText);
    std::string positionText;

    while (std::getline(positionsStream, positionText, ',')) {
        if (positionText.empty()) {
            continue;
        }

        int position = 0;

        if (!parseNonNegativeInt(positionText, position)) {
            return false;
        }

        posting.positions.push_back(position);
    }

    // There must be one position for every occurrence of the term.
    if (posting.positions.size() !=
        static_cast<size_t>(posting.termFrequency)) {
        return false;
    }

    // Positions must be in ascending order.
    if (!std::is_sorted(
            posting.positions.begin(),
            posting.positions.end())) {
        return false;
    }

    return true;
}

} // namespace

bool IndexReader::load(
    InvertedIndex& index,
    const std::string& filename
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    enum class Section {
        INDEX,
        DOCUMENT_LENGTHS,
        DOCUMENT_FREQUENCIES,
        TOTAL_DOCUMENTS,
        AVERAGE_DOCUMENT_LENGTH
    };

    Section section = Section::INDEX;

    int totalDocuments = -1;
    double averageDocumentLength = -1.0;

    std::unordered_map<int, int> documentLengths;

    std::unordered_map<std::string, int> documentFrequencies;

    std::unordered_map<std::string, std::vector<Posting>> loadedIndex;

    bool sawDocumentLengths = false;
    bool sawDocumentFrequencies = false;
    bool sawTotalDocuments = false;
    bool sawAverageDocumentLength = false;

    std::string line;

    while (std::getline(file, line)) {

        // Identify section markers.
        if (line == "DOCUMENT_LENGTHS") {
            if (sawDocumentLengths) {
                return false;
            }

            sawDocumentLengths = true;
            section = Section::DOCUMENT_LENGTHS;
            continue;
        }

        if (line == "DOCUMENT_FREQUENCIES") {
            if (sawDocumentFrequencies) {
                return false;
            }

            sawDocumentFrequencies = true;
            section = Section::DOCUMENT_FREQUENCIES;
            continue;
        }

        if (line == "TOTAL_DOCUMENTS") {
            if (sawTotalDocuments) {
                return false;
            }

            sawTotalDocuments = true;
            section = Section::TOTAL_DOCUMENTS;
            continue;
        }

        if (line == "AVERAGE_DOCUMENT_LENGTH") {
            if (sawAverageDocumentLength) {
                return false;
            }

            sawAverageDocumentLength = true;
            section = Section::AVERAGE_DOCUMENT_LENGTH;
            continue;
        }

        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);

        // Read total document count.
        if (section == Section::TOTAL_DOCUMENTS) {
            std::string value;
            std::string extra;

            if (!(ss >> value) ||
                (ss >> extra) ||
                !parseNonNegativeInt(value, totalDocuments)) {
                return false;
            }

            continue;
        }

        // Read average document length.
        if (section == Section::AVERAGE_DOCUMENT_LENGTH) {
            std::string value;
            std::string extra;

            if (!(ss >> value) || (ss >> extra)) {
                return false;
            }

            try {
                size_t parsed = 0;

                averageDocumentLength = std::stod(value, &parsed);

                if (parsed != value.size() ||
                    !std::isfinite(averageDocumentLength) ||
                    averageDocumentLength < 0.0) {
                    return false;
                }
            } catch (...) {
                return false;
            }

            continue;
        }

        // Read document lengths.
        if (section == Section::DOCUMENT_LENGTHS) {
            std::string idText;
            std::string lengthText;
            std::string extra;

            int documentId = 0;
            int documentLength = 0;

            if (!(ss >> idText >> lengthText) ||
                (ss >> extra) ||
                !parseNonNegativeInt(idText, documentId) ||
                !parseNonNegativeInt(lengthText, documentLength) ||
                documentLengths.count(documentId) != 0) {
                return false;
            }

            documentLengths.emplace(documentId, documentLength);
            continue;
        }

        // Read document frequencies.
        if (section == Section::DOCUMENT_FREQUENCIES) {
            std::string word;
            std::string frequencyText;
            std::string extra;

            int frequency = 0;

            if (!(ss >> word >> frequencyText) ||
                (ss >> extra) ||
                !parseNonNegativeInt(frequencyText, frequency) ||
                frequency <= 0 ||
                documentFrequencies.count(word) != 0) {
                return false;
            }

            documentFrequencies.emplace(word, frequency);
            continue;
        }

        // Read inverted-index entries and their postings.
        std::string word;

        if (!(ss >> word) || loadedIndex.count(word) != 0) {
            return false;
        }

        std::string postingToken;
        std::vector<Posting> postings;

        while (ss >> postingToken) {
            Posting posting{};

            if (!parsePosting(postingToken, posting)) {
                return false;
            }

            postings.push_back(std::move(posting));
        }

        if (postings.empty()) {
            return false;
        }

        // Keep postings sorted by document ID.
        std::sort(
            postings.begin(),
            postings.end(),
            [](const Posting& a, const Posting& b) {
                return a.documentId < b.documentId;
            }
        );

        // A term must not have duplicate postings for the same document.
        for (size_t i = 1; i < postings.size(); ++i) {
            if (postings[i - 1].documentId ==
                postings[i].documentId) {
                return false;
            }
        }

        loadedIndex.emplace(word, std::move(postings));
    }

    // Check that the file was read successfully and all sections exist.
    if (file.bad() ||
        !sawDocumentLengths ||
        !sawDocumentFrequencies ||
        !sawTotalDocuments ||
        !sawAverageDocumentLength ||
        totalDocuments < 0 ||
        averageDocumentLength < 0.0 ||
        documentLengths.size() !=
            static_cast<size_t>(totalDocuments)) {
        return false;
    }

    // Validate document lengths and calculate their total.
    long long totalLength = 0;

    for (const auto& entry : documentLengths) {
        totalLength += entry.second;
    }

    const double expectedAverage =
        totalDocuments == 0
            ? 0.0
            : static_cast<double>(totalLength) / totalDocuments;

    if (std::abs(expectedAverage - averageDocumentLength) > 1e-9) {
        return false;
    }

    // The number of document-frequency entries must match the index terms.
    if (documentFrequencies.size() != loadedIndex.size()) {
        return false;
    }

    // Validate each term's postings and document frequency.
    for (const auto& entry : loadedIndex) {
        auto frequencyIt = documentFrequencies.find(entry.first);

        if (frequencyIt == documentFrequencies.end() ||
            frequencyIt->second !=
                static_cast<int>(entry.second.size())) {
            return false;
        }

        for (const Posting& posting : entry.second) {
            auto lengthIt = documentLengths.find(posting.documentId);

            if (lengthIt == documentLengths.end()) {
                return false;
            }

            // No term position can be outside its document.
            for (int position : posting.positions) {
                if (position >= lengthIt->second) {
                    return false;
                }
            }
        }
    }

    // Only update the original index after every validation succeeds.
    index.loadData(
        loadedIndex,
        documentLengths,
        documentFrequencies,
        totalDocuments,
        averageDocumentLength
    );

    return true;
}