//
// Created by radko on 10/16/16.
//

#include "parse.h"
#include <fstream>
#include <utility>

using std::endl;

Parser::Parser(std::string _filename) {
    filename = std::move(_filename);
}


Model* Reader::parse(Model *m) {
    int d, ds, s, ss, f, fs, h, hs, n;
    std::ifstream ifs(filename);

    ifs >> d >> ds >> s >> ss >> f >> fs >> h >> hs >> n;

    int beastID = 0;
    m->setInfo(n, ds, ss, fs, hs);
    for (int i = 0; i < d; ++i, ++beastID) {
        long long x,y,z,vx,vy,vz;
        ifs >> x >> y >> z >> vx >> vy >> vz;

        Beast* b = new Dragonfly(beastID, x, y, z, vx, vy, vz, ds);
        m->addBeast(b);
    }

    for (int i = 0; i < s; ++i, ++beastID) {
        long long x,y,z,vx,vy,vz;
        ifs >> x >> y >> z >> vx >> vy >> vz;

        Beast* b = new Stork(beastID, x, y, z, vx ,vy, vz, ss);
        m->addBeast(b);
    }

    for (int i = 0; i < f; ++i, ++beastID) {
        long long x,y,vx,vy;
        ifs >> x >> y >> vx >> vy;

        Beast* b = new Frog(beastID, x, y, vx, vy, fs);
        m->addBeast(b);
    }

    for (int i = 0; i < h; ++i, ++beastID) {
        long long x,y,vx,vy;
        ifs >> x >> y >> vx >> vy;

        Beast* b = new Hedgehog(beastID, x, y, vx, vy, hs);
        m->addBeast(b);
    }

    ifs.close();

    return m;
}

Model* Writer::parse(Model *m) {
    long long d, s, f, h;
    std::ofstream ofs(filename);

    std::vector<long long> v = m->getInfo();

    ofs <<  (d = v[0]) << " " << v[1] << " " <<
            (s = v[2]) << " " << v[3] << " " <<
            (f = v[4]) << " " << v[5] << " " <<
            (h = v[6]) << " " << v[7] << " " <<
            v[8] << endl;

    int i = 9;
    for (int j = 0; j < d; i+=6, ++j) {
        ofs << v[i] << " " << v[i+1] << " " << v[i+2] << " " <<
             v[i+3] << " " << v[i+4] << " " << v[i+5] << endl;
    }

    for (int j = 0; j < s; i+=6, ++j) {
        ofs << v[i] << " " << v[i+1] << " " << v[i+2] << " " <<
             v[i+3] << " " << v[i+4] << " " << v[i+5] << endl;
    }

    for (int j = 0; j < f; i+=4, ++j) {
        ofs << v[i] << " " << v[i+1] << " " <<
             v[i+2] << " " << v[i+3] << " " << endl;
    }

    for (int j = 0; j < h; i+=4, ++j) {
        ofs << v[i] << " " << v[i+1] << " " <<
             v[i+2] << " " << v[i+3] << " " << endl;
    }

    ofs.close();

    return m;
}

