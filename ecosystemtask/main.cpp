#include "parse.h"

int main() {

    Parser* in = new Reader("input.txt");
    auto* m = new Model(1000000, 1000000, 1000000);

    in->parse(m);

    m->run();

    Parser* out = new Writer("output.txt");

    out->parse(m);

    return 0;
}