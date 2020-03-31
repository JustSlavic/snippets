//
// Created by radko on 11/20/16.
//

#include <utility>
#include "warehouseDocument.h"
#include "company.h"

WarehouseDoc::WarehouseDoc(long long _number, MovementType _movementType, string _date, string _agent,
                           vector<Good> &_goods, shared_ptr<Company> company)
        :Document(_number, _movementType, DocumentType::WAREHOUSE, std::move(_date), std::move(_agent),
        std::move(company)) {
    goods = _goods;
}

void WarehouseDoc::process() {
    vector<Good> tmp = goods;

    for (auto&& item : tmp) {
        item.sum *= static_cast<int>(movementType);
        item.quantity *= static_cast<int>(movementType);
    }

    company->moveGoods(agent, tmp);
}



