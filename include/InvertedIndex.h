#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Posting.h"

class InvertedIndex {
private:
    std::unordered_map<std::string, std::vector<Posting>> index;

    std::unordered_map<int, int> documentLengths;

    double averageDocumentLength;

    int totalDocumentLength = 0;

    int totalDocuments = 0;

    std::unordered_map<std::string, int> documentFrequency;

public:
    void addDocument(
        int documentId,
        const std::vector<std::string>& tokens
    );


    void merge(const InvertedIndex& other);

    std::vector<Posting> search(
        const std::string& word
    ) const ;

   int getDocumentLength(int documentId) const;
double getAverageDocumentLength() const;
int getDocumentFrequency(const std::string& word) const;
    int getTotalDocuments() const;

 const std::unordered_map<std::string, std::vector<Posting>>& getIndex() const;

  const std::unordered_map<int, int>& getDocumentLengths() const;

  const std::unordered_map<std::string, int>& getDocumentFrequencies() const;

  void loadData(
    const std::unordered_map<std::string, std::vector<Posting>>& loadedIndex,
    const std::unordered_map<int, int>& loadedDocumentLengths,
    const std::unordered_map<std::string, int>& loadedDocumentFrequencies,
    int loadedTotalDocuments,
    double loadedAverageDocumentLength
);
};
