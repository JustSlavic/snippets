//
// Created by radko on 11/25/16.
//

#include "parser.h"
#include "bankDocument.h"
#include "warehouseDocument.h"
#include <fstream>
#include <iomanip>
#include <cmath>

void Reader::parse(shared_ptr<Company> company) {

    std::ifstream ifs(fileName);

    unsigned int n;
    string docType;
    long long number;
    string date;
    string time;
    string agent;

    ifs >> n;
    for (int i = 0; i < n; ++i) {

        ifs >> docType >> number >> date >> time >> agent;

        while ((*(agent.end()-1)) != '`') {
            string b;
            ifs >> b;
            agent += " " + b;
        }

        agent.erase(agent.begin(), agent.begin()+1);
        agent.erase(agent.end()-1, agent.end());

        date += " ";
        date += time;

        if (docType[0] == 'B') {
            string account;
            double sum;

            ifs >> account >> sum;

            auto accountSum = static_cast<long long>(round(sum * 100));

            MovementType movement;

            if (docType[1] == 'W') {
                movement = MovementType :: WITHDRAW;
            }

            if (docType[1] == 'I') {
                movement = MovementType :: INCOME;
            }

            auto bankDoc = std::make_shared<BankDoc>(number, movement, date, agent, account, accountSum, company);
            company->pushDocument(bankDoc);
        }

        if (docType[0] == 'W') {
            unsigned int goodsNumber;
            ifs >> goodsNumber;

            vector<Good> goodsVector;
            goodsVector.reserve(goodsNumber);

            for (int j = 0; j < goodsNumber; ++j) {
                string goodName;
                double goodNumber;
                double goodSum;

                ifs >> goodName;

                while ((*(goodName.end()-1)) != '`') {
                    string b;
                    ifs >> b;
                    goodName += " " + b;
                }

                goodName.erase(goodName.begin(), goodName.begin()+1);
                goodName.erase(goodName.end()-1, goodName.end());

                ifs >> goodNumber >> goodSum;

                auto gS = static_cast<int>(round(goodSum * 100));
                auto integralGoodNumber = static_cast<int>(round(goodNumber * 1000));

                goodsVector.emplace_back(Good{goodName, integralGoodNumber, gS});
            }

            MovementType movement;

            if (docType[1] == 'W') {
                movement = MovementType :: WITHDRAW;
            }
            if (docType[1] == 'I') {
                movement = MovementType :: INCOME;
            }

            auto warehouseDoc = std::make_shared<WarehouseDoc>(number, movement, date, agent, goodsVector, company);
            company->pushDocument(warehouseDoc);
        }
    }
    ifs.close();
}

void Writer::parse(shared_ptr<Company> company) {
    std::ofstream ofs(fileName);

    ofs << company->generateReport();

    ofs.close();
}
