//
// Created by radko on 11/20/16.
//

#pragma once

#include "document.h"

using std::vector;

class WarehouseDoc :public Document {
public:
    WarehouseDoc(long long _number, MovementType _movementType, string _date, string _agent,
                 vector<Good>& _goods, shared_ptr<Company> company);

    void process() override;

protected:
    vector<Good> goods;
};
