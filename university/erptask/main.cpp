#include <iostream>
#include "parser.h"

int main() {
    auto b = std::make_shared<Bank>();
    b->transaction("11111111111111111111", 10000000);

    auto c = std::make_shared<Company>(b);

    auto reader = std::make_unique<Reader>("input.txt");
    auto writer = std::make_unique<Writer>("output.txt");

    reader->parse(c);

    c->processAllDocuments();

    writer->parse(c);

    return 0;
}