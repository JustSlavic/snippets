//
// Created by radko on 29.09.18.
//

#pragma once

#include <NTL/ZZ_pXFactoring.h>
#include <NTL/vector.h>
#include <vector>

using NTL::ZZ;
using NTL::ZZ_p;
using NTL::ZZ_pX;
using NTL::Vec;

class ReedSolomonCode {
    /*
     * I made code class singleton due to the state-dependent nature of the NTL
     */
public:
    static ReedSolomonCode& initialize(const unsigned& _q, const unsigned& _k);
    static ReedSolomonCode& instance();
    /*
     *  IN:  vector of length k
     *  OUT: polynomial (or vector) of degree (length) n
     */
    Vec<ZZ_p> encode(const Vec<ZZ_p>&);
    /*
     *  IN:  polynomial (or vector) of degree (length) n
     *  OUT: vector of length k (or vector of errors)
     */
    Vec<ZZ_p> decode(const Vec<ZZ_p>&);

protected:
    Vec<ZZ_p> build_syndrome(const Vec<ZZ_p>& v);
    unsigned find_number_of_errors(const Vec<ZZ_p>& v);
    ZZ_pX build_lambda(const Vec<ZZ_p>& v, unsigned t);
    std::vector<ZZ_p> find_error_locators(const Vec<ZZ_p>& v, unsigned t);
    std::vector<unsigned> find_error_locations(const std::vector<ZZ_p>& v);
    unsigned find_power(const ZZ_p& x);
    Vec<ZZ_p> find_error_values(const Vec<ZZ_p>& v, unsigned t, const std::vector<ZZ_p>& xs);

    Vec<ZZ_p> ber_mes(const Vec<ZZ_p>& s, int t);
    std::vector<ZZ_p> find_roots(const ZZ_pX& f);
    ZZ_p find_primitive() const;
protected:
    unsigned q;
    unsigned k;
    unsigned n;
    ZZ_p alpha;

private:
    ReedSolomonCode() = default;
    ~ReedSolomonCode() = default;
};
