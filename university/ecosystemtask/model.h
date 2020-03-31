//
// Created by radko on 07.10.16.
//

#pragma once

#include "events.h"
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class Event;

enum ModelState {
    processing,
    stopped
};

class Model {
public:
    Model() : state(processing) {}
    Model(long long _mX, long long _mY, long long _mZ);

    void addBeast(Beast* beast);
    void delBeast(Beast* target, Event* doNotDelMe);
    void setInfo(int n, int _dragon_sens, int _stork_sens, int _frog_sens, int _hedgehog_sens);
    vector<long long> getInfo();

    void run();
    void stopModeling();
    void turn();
private:
    void analiseEvents();
    void handleAll();
    void clearEvents();
    void moveBeasts();
    bool ranOut(Beast *_beast);
    void updateAllBeastStates();
//Data
    long long maxXCoord = 0, maxYCoord = 0, maxZCoord = 0;
    int maxTurns = 0, turnsPassed = 0;
    int dragonflySens = 0, storkSens = 0, frogSens = 0, hedgehogSens = 0;
    vector<unique_ptr<Beast>> beasts;
    vector<unique_ptr<Event>> events;
    ModelState state;
};
