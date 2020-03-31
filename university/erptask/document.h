//
// Created by radko on 11/20/16.
//

#pragma once

#include <utility>
#include <vector>
#include <memory>
#include "datetime.h"
#include "bank.h"

using std::string;
using std::vector;
using std::shared_ptr;

class Company;

struct Good{
    string goodName;
    int quantity;
    int sum;
};

enum class MovementType {
    INCOME = 1,
    WITHDRAW = -1
};

string movementToString(MovementType m);

enum class DocumentType {
    BANK,
    WAREHOUSE
};

string documentTypeToString(DocumentType t);

class Document {
public:
    DocumentType getDocumentType() const { return documentType; }

    long long int getId() const { return id; }

    MovementType getMovementType() const { return movementType; }

    const DateTime& getDate() const { return date; }

    virtual void process() = 0;

protected:
    Document(long long _number, MovementType _movementType, DocumentType _type, string _date, string _agent, shared_ptr<Company> _company)
            :id(_number), movementType(_movementType), documentType(_type), date(std::move(_date)), agent(std::move(_agent)), company(
            std::move(_company)) {}

protected: //Data
    long long id;
    MovementType movementType;
    DocumentType documentType;
    DateTime date;
    string agent;
    shared_ptr<Company> company;
};

