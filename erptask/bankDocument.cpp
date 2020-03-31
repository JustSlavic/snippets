//
// Created by radko on 11/20/16.
//

#include <utility>
#include "bankDocument.h"
#include "company.h"

BankDoc::BankDoc(long long _number, MovementType _movementType, string _date, string _agent, string _accountNumber,
                 unsigned int _sum, shared_ptr<Company> company)
        :Document(_number, _movementType, DocumentType::BANK, std::move(_date), std::move(_agent), std::move(company)) {
    accountNumber = std::move(_accountNumber);
    sum = _sum;
}

void BankDoc::process() {
    company->transferMoney(agent, accountNumber, sum * static_cast<int>(movementType));
}



