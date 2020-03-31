//
// Created by radko on 11/20/16.
//

#pragma once

#include "document.h"

class Document;
typedef string bankAccount;

class BankDoc :public Document {
public:
    BankDoc(long long _number, MovementType _movementType, string _date, string _agent, string _accountNumber,
            unsigned int _sum, shared_ptr<Company> company);

    void process() override;

protected:
    bankAccount accountNumber;
    long long int sum;
};

