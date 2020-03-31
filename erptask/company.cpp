//
// Created by radko on 11/25/16.
//

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include "company.h"

void Company::pushDocument(shared_ptr<Document> document) {
    docs.emplace_back(std::make_pair(document->getDate(), document));
}

void Company::transferMoney(string _agent, bankAccount _bankAccount, long long int _amount) {
    bank->transaction(std::move(_bankAccount), _amount);
    changeDebt(std::move(_agent), -_amount);
}

void Company::changeDebt(debtor agent, long long int amount) {
    auto d = debts.find(agent);
    if (d == debts.end()) {
        debts.emplace(agent, amount);
    } else {
        d->second += amount;
    }
}

void Company::moveGoods(debtor agent, vector<Good> _goods) {
    for (auto&& item : _goods) {
        auto g = warehouse.find(item.goodName);
        if (g != warehouse.end()) {
            g->second += item.quantity;
        } else {
            warehouse.emplace(item.goodName, item.quantity);
        }
        changeDebt(agent, -item.sum);
    }
}

void Company::processAllDocuments() {
    std::sort(docs.begin(), docs.end(), [](pair<DateTime, shared_ptr<Document>> const & a, pair<DateTime, shared_ptr<Document>> const & b) {
        return a.first < b.first;
    });

    for (auto&& doc : docs) {
        doc.second->process();
        last = doc.second;
        if (!check()) {
            success = false;
            break;
        }
    }
}

bool Company::check() {
    if (!bank->check().second) {
        for (auto&& item : warehouse) {
            if (item.second<0) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

string Company::generateReport() {
    std::stringstream s;

    if (success) {
        s << "OK " << to_string(debts.size()) << "\n";

        vector<std::pair<debtor, int>> v;

        for (auto&& item : debts) {
            v.emplace_back(item);
        }

        std::sort(v.begin(), v.end(), [](std::pair<debtor, int> const & a, std::pair<debtor, int> const & b) {
            return a.second != b.second ? a.second > b.second : a.first < b.first;
        });

        for (auto&& item : v) {
            s << '`' << item.first << (item.second < 0  && item.second > -100 ? "` -" : "` ") << item.second/100 << '.'
                    << std::setw(2) << std::setfill('0') << abs(item.second % 100) << std::endl;
        }
    } else if (last->getDocumentType() == DocumentType::BANK) {
        auto errPair = bank->check();

        s << "ERROR B" << movementToString(last->getMovementType())
                << " " << last->getId() << "\n" << errPair.first << (errPair.second < 0 && errPair.second > -100 ? " -" : " ")
                << errPair.second/100 << '.' << std::setw(2) << std::setfill('0')
                << llabs(errPair.second % 100) << std::endl;
    } else if (last->getDocumentType() == DocumentType::WAREHOUSE) {
        s << "ERROR W" << movementToString(last->getMovementType())
                << " " << last->getId() << " ";

        vector<std::pair<string, int>> v;

        for (auto&& item : warehouse) {
            if (item.second < 0) {
                v.emplace_back(item);
            }
        }

        std::sort(v.begin(), v.end(), [](std::pair<debtor, int> const & a, std::pair<debtor, int> const & b) {
            return a.first == b.first ? a.second > b.second : a.first < b.first;
        });

        s << v.size() << std::endl;

        for (auto&& item : v) {
            s << '`' << item.first << (item.second < 0 && item.second > -1000 ? "` -" : "` ") << item.second/1000 << '.'
                    << std::setw(3) << std::setfill('0') << abs(item.second % 1000) << std::endl;
        }
    }

    return s.str();
}










