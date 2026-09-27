#include "Tokenizer.h"

#include <sstream>
#include <cctype>
#include <unordered_set>

std::vector<std::string> Tokenizer::tokenize(
    const std::string& text
) {
    std::vector<std::string> tokens;

    std::unordered_set<std::string> stopWords = {
        "a",
        "an",
        "the",
        "is",
        "are",
        "of",
        "to",
        "in",
        "and",
        "for"
    };

    std::stringstream ss(text);

    std::string word;

    while (ss >> word) {

        for (char& c : word) {

            if (std::ispunct(
                    static_cast<unsigned char>(c))) {

                c = ' ';
            }
            else {
                c = std::tolower(
                    static_cast<unsigned char>(c));
            }
        }

        std::stringstream wordStream(word);

        std::string cleanWord;

        while (wordStream >> cleanWord) {

            if (stopWords.find(cleanWord) ==
                stopWords.end()) {

                tokens.push_back(cleanWord);
            }
        }
    }

    return tokens;
}