//
// Created by radko on 12/25/16.
//

#pragma once

#include <map>
#include <string>

using std::map;
using std::string;
using std::pair;

typedef string bankAccount;

class Bank {
public:

    long long int transaction(bankAccount _account, long long int _money);

    pair<bankAccount, long long int> check();

protected:
    map<bankAccount, long long> accounts;
};
