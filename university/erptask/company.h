//
// Created by radko on 11/25/16.
//

#pragma once

#include "document.h"
#include <memory>
#include <map>
#include <iostream>
#include <utility>

using std::shared_ptr;
using std::vector;
using std::map;
using std::pair;

class Document;
struct Good;

typedef string debtor;

class Company {
public:

    explicit Company(shared_ptr<Bank> _bank) :bank(std::move(_bank)) {}

    void pushDocument(shared_ptr<Document> document);

    void transferMoney(string _agent, bankAccount _bankAccount, long long int _amount);

    void moveGoods(debtor agent, vector <Good> _goods);

    void processAllDocuments();

    string generateReport();

private:
    void changeDebt(debtor agent, long long int amount);

    bool check();

private:
    shared_ptr<Bank> bank;
    map<string, int> warehouse;
    map<debtor, int> debts;
    vector<pair<DateTime, shared_ptr<Document>>> docs;
    shared_ptr<Document> last;
    bool success = true;
};
