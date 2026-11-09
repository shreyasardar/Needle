#include "IndexWriter.h"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <string>

bool IndexWriter::save(
    const InvertedIndex& index,
    const std::string& filename
) {
    // Write to a temporary file first.
    const std::string tempFilename = filename + ".tmp";

    std::ofstream file(
        tempFilename,
        std::ios::out | std::ios::trunc
    );

    if (!file.is_open()) {
        return false;
    }

    file << std::setprecision(17);

    const auto& data = index.getIndex();

    // Save inverted index and postings.
    for (const auto& entry : data) {
        file << entry.first << " ";

        for (const auto& posting : entry.second) {
            file << posting.documentId << ":"
                 << posting.termFrequency << ":";

            for (int position : posting.positions) {
                file << position << ",";
            }

            file << " ";
        }

        file << "\n";
    }

    // Save document lengths.
    file << "\nDOCUMENT_LENGTHS\n";

    const auto& lengths = index.getDocumentLengths();

    for (const auto& entry : lengths) {
        file << entry.first << " "
             << entry.second << "\n";
    }

    // Save document frequencies.
    file << "\nDOCUMENT_FREQUENCIES\n";

    const auto& frequencies = index.getDocumentFrequencies();

    for (const auto& entry : frequencies) {
        file << entry.first << " "
             << entry.second << "\n";
    }

    // Save total documents.
    file << "\nTOTAL_DOCUMENTS\n";
    file << index.getTotalDocuments() << "\n";

    // Save average document length.
    file << "\nAVERAGE_DOCUMENT_LENGTH\n";
    file << index.getAverageDocumentLength() << "\n";

    // Verify that writing succeeded.
    file.flush();

    if (!file.good()) {
        file.close();
        std::remove(tempFilename.c_str());
        return false;
    }

    file.close();

    if (file.fail()) {
        std::remove(tempFilename.c_str());
        return false;
    }

    // Atomically replace the old index with the new one.
    // On Linux, rename replaces an existing destination atomically
    // when both files are on the same filesystem.
    if (std::rename(
            tempFilename.c_str(),
            filename.c_str()
        ) != 0) {

        std::remove(tempFilename.c_str());
        return false;
    }

    return true;
}