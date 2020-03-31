//
// Created by radko on 11/25/16.
//

#pragma once

#include <utility>
#include "company.h"

using std::string;

class Parser {
public:
    virtual void parse(shared_ptr<Company> company) = 0;

protected:
    explicit Parser(string _filename) :fileName(std::move(_filename)) {}

protected: //Data
    string fileName;
};

class Reader :public Parser {
public:
    explicit Reader(string _filename) :Parser(std::move(_filename)) {}

    void parse(shared_ptr<Company> company) override;
};

class Writer :public Parser {
public:
    explicit Writer(string _filename) :Parser(std::move(_filename)) {}

    void parse(shared_ptr<Company> company) override;
};
