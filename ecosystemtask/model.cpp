//
// Created by radko on 07.10.16.
//

#include "model.h"

using std::vector;
using std::unique_ptr;

void Model::analiseEvents() {
    for (auto &&currentBeast : beasts) {
        Beast* victim = nullptr;
        Beast* predator = nullptr;

        for (auto &&scannedBeast : beasts) {
            if (currentBeast->canSense(scannedBeast.get())) {
                if (currentBeast->isFood(scannedBeast.get())) {
                    if (currentBeast->getState() == hanging) {
                        if (victim == nullptr ||
                                currentBeast->getDistance(victim) > currentBeast->getDistance(scannedBeast.get())) {
                            victim = scannedBeast.get();
                        }
                    } else if (currentBeast->isInteractWith(scannedBeast.get())) {
                        if (currentBeast->getState() == chasing) {
                            events.emplace_back(new MoveTo(currentBeast.get(),scannedBeast.get()));
                        }
                    }
                }

                if (currentBeast->isAfraidOf(scannedBeast.get())) { // if scanned beast is dangerous for current one
                    if (predator == nullptr || currentBeast->getDistance(predator) > currentBeast->getDistance(scannedBeast.get())) {
                        predator = scannedBeast.get(); // if there is no predator yet or previous predator is further then current one
                    }
                }
            }

            // eating
            if (currentBeast->isFood(scannedBeast.get()) && currentBeast->canEat(scannedBeast.get())) {
                events.emplace_back(new MarkToDelete(scannedBeast.get(),this));
            }
        }

        // now we remember the nearest victim and predator and choose one
        if (predator == nullptr) {
            if (victim != nullptr) {
                events.emplace_back(new MoveTo(currentBeast.get(),victim));
            }
        } else {
            events.emplace_back(new MoveFrom(currentBeast.get(),predator));
        }

        // if runner or chaser moved out of model => stop modeling
        if (ranOut(currentBeast.get()) && currentBeast->getState() != hanging) {
            events.emplace_back(new StopProcessing(this));
        }

        // any beast which ran out and do nothing must be EXTERMINATED
        if (ranOut(currentBeast.get()) && currentBeast->getState() == hanging) {
            events.emplace_back(new MarkToDelete(currentBeast.get(),this));
        }
    }

    // if end of time => push stop processing
    if (turnsPassed == maxTurns) {
        events.emplace_back(new StopProcessing(this));
    }
}

void Model::handleAll() {
    for (auto &&event : events) {
        if (event->getType() != markToDelete)
            event->handle();
    }

    //size of vector changes in loop, so I can't use "auto"
    for (auto it = events.begin(); it != events.end(); ++it) {
        if ((*it)->getType() == markToDelete) {
            (*it)->handle();
        }
    }
}

void Model::delBeast(Beast* target, Event* doNotDelMe) {
    for (auto it = beasts.begin(); it != beasts.end(); ++it) {
        if (it->get() == target) {
            beasts.erase(it);
            break;
        }
    }
    for (auto it = events.end()-1; ; --it) {
        if (it->get() == doNotDelMe) break;
        if ((*it)->getBeast() == target && (*it)->getType() == markToDelete) {
            events.erase(it);
        }
    }
    for (auto &&beast : beasts) {
        if (beast->getTarget() == target) {
            beast->toNullState();
        }
    }
}

void Model::turn() {
    ++turnsPassed;
    moveBeasts();
    updateAllBeastStates();
    analiseEvents();
    handleAll();
    clearEvents();
}

void Model::moveBeasts() {
    for (auto &&beast : beasts) {
        beast->move();
    }
}

void Model::addBeast(Beast *beast) {
    beasts.emplace_back(beast);
}

void Model::setInfo(int n, int _dragon_sens, int _stork_sens, int _frog_sens, int _hedgehog_sens) {
    maxTurns = n;
    dragonflySens = _dragon_sens;
    storkSens = _stork_sens;
    frogSens = _frog_sens;
    hedgehogSens = _hedgehog_sens;
}

vector<long long> Model::getInfo() {
    vector<long long> v;

    int d = 0;
    int s = 0;
    int f = 0;
    int h = 0;
    for (auto &&beast : beasts) {
        switch (beast->getSpecies()) {
        case dragonfly: ++d;
            break;
        case stork: ++s;
            break;
        case frog: ++f;
            break;
        case hedgehog: ++h;
            break;
        default:
            break;
        }
    }

    v.emplace_back(d);
    v.emplace_back(dragonflySens);
    v.emplace_back(s);
    v.emplace_back(storkSens);
    v.emplace_back(f);
    v.emplace_back(frogSens);
    v.emplace_back(h);
    v.emplace_back(hedgehogSens);
    v.emplace_back(turnsPassed);


    for (auto &&beast : beasts) {
        if (beast->getSpecies() == dragonfly) {
            v.emplace_back(beast->getX());
            v.emplace_back(beast->getY());
            v.emplace_back(beast->getZ());
            v.emplace_back(beast->getVX());
            v.emplace_back(beast->getVY());
            v.emplace_back(beast->getVZ());
        }
    }
    for (auto &&beast : beasts) {
        if (beast->getSpecies() == stork) {
            v.emplace_back(beast->getX());
            v.emplace_back(beast->getY());
            v.emplace_back(beast->getZ());
            v.emplace_back(beast->getVX());
            v.emplace_back(beast->getVY());
            v.emplace_back(beast->getVZ());
        }
    }
    for (auto &&beast : beasts) {
        if (beast->getSpecies() == frog) {
            v.emplace_back(beast->getX());
            v.emplace_back(beast->getY());
            v.emplace_back(beast->getVX());
            v.emplace_back(beast->getVY());
        }
    }
    for (auto &&beast : beasts) {
        if (beast->getSpecies() == hedgehog) {
            v.emplace_back(beast->getX());
            v.emplace_back(beast->getY());
            v.emplace_back(beast->getVX());
            v.emplace_back(beast->getVY());
        }
    }

    return v;
}

Model::Model(long long _mX, long long _mY, long long _mZ) {
    maxXCoord = _mX;
    maxYCoord = _mY;
    maxZCoord = _mZ;
    state = processing;
    turnsPassed = 0;
}

bool Model::ranOut(Beast *_beast) {
    return  _beast->getX() < 0 || _beast->getX() > maxXCoord ||
            _beast->getY() < 0 || _beast->getY() > maxYCoord ||
            _beast->getZ() < 0 || _beast->getZ() > maxZCoord;
}

void Model::run() {
    while (state == processing) {
        turn();
    }
}

void Model::updateAllBeastStates() {
    for (auto &&beast : beasts) {
        beast->updateState();
    }
}

void Model::clearEvents() {
    events.clear();
}

void Model::stopModeling() {
    state = stopped;
}






