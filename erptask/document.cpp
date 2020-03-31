//
// Created by radko on 12/12/16.
//

#include "document.h"

string movementToString(MovementType m) {
    switch (m) {
        case MovementType::INCOME : return "I";
        case MovementType::WITHDRAW : return "W";
    }
}

string documentTypeToString(DocumentType t) {
    switch (t) {
        case DocumentType::BANK : return "B";
        case DocumentType::WAREHOUSE : return "W";
    }
}


