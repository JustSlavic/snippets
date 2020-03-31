//
// Created by radko on 12/25/16.
//

#include <iostream>
#include "bank.h"

long long int Bank::transaction(bankAccount _account, long long int _money) {
    auto acc = accounts.find(_account);

    if (acc == accounts.end()) {
        accounts.emplace(_account, _money);
    } else {
        acc->second += _money;
    }

    return accounts.find(_account)->second;
}

pair<bankAccount, long long int> Bank::check() {
    for (auto&& account : accounts) {
        if (account.second < 0) {
            return account;
        }
    }

    return std::make_pair<bankAccount, long long>("no error", 0);
}




