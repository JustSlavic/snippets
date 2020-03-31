#include <iostream>
#include <ReedSolomonCode.hpp>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/ZZ_pEX.h>
#include <fstream>

using namespace std;
using namespace NTL;

int main() {
    ifstream ifs("data.init");

    unsigned q, k;
    ifs >> q >> k;

    auto& coder = ReedSolomonCode::initialize(q, k); // set modulo p

    ZZ_pX f;
    ifs >> f;

    auto v = conv<NTL::Vec<ZZ_p>>(f);

    cout << "X: " << v << endl;

    auto c = coder.encode(v);
    cout << "C: " << c << endl;


    c[0] = 0;
    c[3] = 0;
    c[5] = 0;
    c[7] = 0;
    c[10] = 0;
    c[12] = 0;
    c[17] = 0;

    cout << "V: " << c << endl;
    auto dc = coder.decode(c);

    cout << "D: " << dc << endl;

    return 0;
}
