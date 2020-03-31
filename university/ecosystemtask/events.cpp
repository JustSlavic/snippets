//
// Created by radko on 10/12/16.
//

#include "events.h"

void MoveTo::handle() {
    pBeast->changeDirectionTo(target);
}

void MoveFrom::handle() {
    pBeast->changeDirectionFrom(target);
}

void MarkToDelete::handle() {
    master->delBeast(pBeast, this);
}

void StopProcessing::handle() {
    master->stopModeling();
}