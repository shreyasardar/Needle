#include <iostream>
#include "Document.h"

int main() {

    Document doc;

    doc.id = 1;
    doc.title = "Distributed Computing";
    doc.text = "Distributed computing is a field of computer science.";

    std::cout << "Needle search engine starting...\n\n";

    std::cout << "Document ID: " << doc.id << '\n';
    std::cout << "Title: " << doc.title << '\n';
    std::cout << "Text: " << doc.text << '\n';

    return 0;
}