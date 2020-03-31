//
// Created by radko on 29.09.18.
//

#include <NTL/matrix.h>
#include <ReedSolomonCode.hpp>
#include <vector>
#include <set>
#include <iostream>

using namespace std;


ReedSolomonCode& ReedSolomonCode::instance() {
    static ReedSolomonCode _instance;
    return _instance;
}

ReedSolomonCode & ReedSolomonCode::initialize(const unsigned& _q, const unsigned& _k) {
    if (_k <= 1 || _k >= _q - 1) {
        cout << "parameter k have to be in [1, q-1]" << endl;
        throw "parameter k have to be in [1, q-1]";
    }

    auto& code = instance();
    ZZ_p::init(ZZ(_q));
    cout << "initialize environment with ZZ_p = ZZ_" << _q << endl;
    code.q = _q;
    code.n = _q - 1;
    cout << "n = " << _q - 1 << endl;
    code.k = _k;
    cout << "k = " << _k << endl;

    ZZ_p alpha = code.find_primitive();

    code.alpha = alpha; // primitive element of the field F_p

    cout << "alpha = " << alpha << " -- primitive element" << endl;

    cout << "initializing ends" << endl;

    return code;
}

NTL::Vec<ZZ_p> ReedSolomonCode::encode(const NTL::Vec<ZZ_p>& x) {
    ZZ_pX f = NTL::conv<ZZ_pX>(x);

    NTL::Vec<ZZ_p> result;
    result.SetLength(q-1);

    for (int i = 0; i < q-1; ++i) {
        result[i] = NTL::eval(f, NTL::power(alpha, i));
    }

    return result;
}

NTL::Mat<NTL::ZZ_p> build_S_matrix(const NTL::Vec<NTL::ZZ_p>& s, unsigned t) {
    NTL::Mat<NTL::ZZ_p> mat;
    mat.SetDims(t, t);

    for (int i = 0; i < t; ++i) {
        for (int j = 0; j < t; ++j) {
            mat[i][j] = s[i + j];
        }
    }

    return mat;
}

unsigned ReedSolomonCode::find_number_of_errors(const NTL::Vec<NTL::ZZ_p>& v) {
    NTL::Vec<ZZ_p> s = build_syndrome(v);

    for (unsigned i = (n-k)/2; i > 0; --i) {
        auto S = build_S_matrix(s, i);

        if (NTL::determinant(S) != 0) {
            return i;
        }
    }

    return 0;
}

std::vector<NTL::ZZ_p> ReedSolomonCode::find_error_locators(const NTL::Vec<NTL::ZZ_p>& v, unsigned t) {
    auto lam = build_lambda(v, t);

    auto xs = find_roots(lam);

    for (auto& x : xs) {
        x = NTL::inv(x);
    }

    return xs;
}

unsigned ReedSolomonCode::find_power(const NTL::ZZ_p& x) {
    for (unsigned i = 0; i < n; ++i) {
        if (NTL::power(alpha, i) == x) {
            return i;
        }
    }

    throw "wtf";
}

std::vector<unsigned> ReedSolomonCode::find_error_locations(const std::vector<NTL::ZZ_p>& v) {
    std::vector<unsigned> locs(v.size());

    for (int i = 0; i < v.size(); ++i) {
        locs[i] = find_power(v[i]);
    }

    return locs;
}

NTL::Mat<NTL::ZZ_p> build_X_matrix(const std::vector<NTL::ZZ_p>& xs, unsigned t) {
    NTL::Mat<NTL::ZZ_p> mat;
    mat.SetDims(t, t);

    for (int i = 0; i < t; ++i) {
        for (int j = 0; j < t; ++j) {
            mat[i][j] = NTL::power(xs[j], i+1);
        }
    }

    return mat;
}

NTL::Vec<NTL::ZZ_p> ReedSolomonCode::find_error_values(const NTL::Vec<NTL::ZZ_p>& v, unsigned t, const std::vector<NTL::ZZ_p>& xs) {
    NTL::Vec<ZZ_p> s = build_syndrome(v);
    s.SetLength(t);

    auto X = build_X_matrix(xs, t);
    X = NTL::inv(X);

    return X*s;
}

NTL::Vec<NTL::ZZ_p> ReedSolomonCode::decode(const NTL::Vec<NTL::ZZ_p>& v) {
    unsigned t = find_number_of_errors(v);

    auto xs = find_error_locators(v, t);

    auto js = find_error_locations(xs);

//    cout << "xs = ";
//    for (auto&& x : xs) {
//        cout << x << ", ";
//    }
//    cout << endl;
//
//    cout << "js = ";
//    for (auto&& x : js) {
//        cout << x << ", ";
//    }
//    cout << endl;

    auto ev = find_error_values(v, t, xs);

    NTL::Vec<NTL::ZZ_p> e;
    e.SetLength(n);

    for (int i = 0; i < t; ++i) {
        e[js[i]] = ev[i];
    }

    return v-e;
}

NTL::Vec<NTL::ZZ_p> ReedSolomonCode::build_syndrome(const NTL::Vec<NTL::ZZ_p>& v) {
    NTL::Vec<NTL::ZZ_p> s;
    s.SetLength(n-k);

    NTL::ZZ_pX V = NTL::conv<NTL::ZZ_pX>(v);

    for (int i = 0; i < n-k; ++i) {
        s[i] = NTL::eval(V, NTL::power(alpha, i+1));
    }

    return s;
}

NTL::ZZ_pX ReedSolomonCode::build_lambda(const NTL::Vec<NTL::ZZ_p>& v, unsigned t) {
    NTL::Vec<ZZ_p> s = build_syndrome(v);

    NTL::Vec<NTL::ZZ_p> rh_vector;
    rh_vector.SetLength(t);

    for (std::size_t i = 0; i < t; ++i) {
        rh_vector[i] = -s[t+i];
    }

    auto S = build_S_matrix(s, t);

    NTL::Vec<NTL::ZZ_p> Lambdas = NTL::inv(S)*rh_vector;

    NTL::ZZ_pX Lambda;
    NTL::SetCoeff(Lambda, 0, 1);
    for (std::size_t i = t; i > 0; --i) {
        NTL::SetCoeff(Lambda, i, Lambdas[t - i]);
    }

    return Lambda;
}

NTL::Vec<ZZ_p> ReedSolomonCode::ber_mes(const NTL::Vec<ZZ_p>& v, int t) {
    auto s = build_syndrome(v);

    ZZ_pX lambda;
    NTL::SetCoeff(lambda, 0, 1);

    ZZ_pX B;
    NTL::SetCoeff(B, 0, 1);

    NTL::Vec<ZZ_p> delta;
    delta.SetLength(1);
    delta.put(0, ZZ_p(1));

    int r = 0;
    int L = 0;

    ZZ_pX __x;
    NTL::SetCoeff(__x, 1, 1);

    do {
        r++;

        ZZ_p tmp;
        tmp = 0;
        for (int j = 0; j < L+1; ++j) {
            tmp += delta[j]*s[r-j];
        }
        delta.SetLength(r+1);
        delta.put(r, tmp);

        if (delta[r] == 0) {
            B = B*__x;
        } else {
            ZZ_pX T = lambda - delta[r]*__x*B;
            if (2*L > r - 1) {
                lambda = T;
                B = B*__x;
            } else {
                B = NTL::inv(NTL::ZZ_p(delta[r]))*lambda;
                lambda = T;
                L = r - L;
            }
        }
    } while (r != 2*t);

    if (NTL::deg(lambda) == L) {
        return NTL::conv<NTL::Vec<ZZ_p>>(lambda);
    } else {
        throw "Cannot perform BerMes algorithm";
    }
}

std::vector<ZZ_p> ReedSolomonCode::find_roots(const ZZ_pX& f) {
    std::vector<ZZ_p> xs;

    for (int i = 0; i < q; ++i) {
        if (NTL::eval(f, ZZ_p(i)) == 0) {
            xs.emplace_back(ZZ_p(i));
        }
    }

    return xs;
}

NTL::ZZ_p ReedSolomonCode::find_primitive() const {
    for (std::size_t alpha = 1; alpha < q; ++alpha) {
        std::size_t a = 1;
        std::set<std::size_t> _ring;
        for (std::size_t i = 0; i < q-1; ++i) {
            _ring.insert(a % q);
            a = (a * alpha) % q;
        }
        if (_ring.size() == q-1) {
            return ZZ_p(alpha);
        }
    }
    return NTL::ZZ_p(0);
}
