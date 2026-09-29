#include "IndexWriter.h"
#include <fstream>

void IndexWriter::save(
    const InvertedIndex& index,
    const std::string& filename
) {
    std::ofstream file(filename);

const auto& data = index.getIndex();

for (const auto& entry : data) {
    file << entry.first << " ";

    for (const auto& posting : entry.second) {
        file << posting.documentId << ":"
             << posting.termFrequency << " ";
    }

    file << "\n";
}
file << "\nDOCUMENT_LENGTHS\n";

const auto& lengths = index.getDocumentLengths();

for (const auto& entry : lengths) {
    file << entry.first << " "
         << entry.second << "\n";
}

file << "\nDOCUMENT_FREQUENCIES\n";

const auto& frequencies = index.getDocumentFrequencies();

for (const auto& entry : frequencies) {
    file << entry.first << " "
         << entry.second << "\n";
}

file << "\nTOTAL_DOCUMENTS\n";
file << index.getTotalDocuments() << "\n";

file << "\nAVERAGE_DOCUMENT_LENGTH\n";
file << index.getAverageDocumentLength() << "\n";
}