//
// Created by radko on 10/16/16.
//

#pragma once

#include "model.h"
#include <string>
#include <utility>

using std::string;

class Parser {
public:
    explicit Parser(std::string _filename);
    virtual Model* parse(Model* m) = 0;

protected:
    string filename;
};

class Reader :public Parser {
public:
    explicit Reader(std::string _filename) :Parser(std::move(_filename)) {}

    Model *parse(Model* m) override;

private:
};

class Writer :public Parser {
public:
    explicit Writer(std::string _filename) :Parser(std::move(_filename)) {}

    Model *parse(Model* m) override;

private:
};

