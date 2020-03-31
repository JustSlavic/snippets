//
// Created by radko on 30.09.16.
//

#pragma once

#include "animals.h"
#include "model.h"
#include <memory>

enum EventType {
    unknown,
    run,
    chase,
    markToDelete,
    stopProcessing
};

class Model;

class Event {
public:
    Event() = default;
    Event(EventType eT, Model* _master) :type(eT), master(_master) {}

    virtual void handle() = 0;

    const Beast* getBeast() const { return pBeast; }
    EventType getType() const { return type; }
protected:
//Data
    EventType type = unknown;
    Beast* pBeast = nullptr;
    Model* master = nullptr;
    const Beast* target = nullptr;
};

class ChangeDirection : public Event {
protected:
    ChangeDirection(Beast* active, const Beast* passive) {
        pBeast = active;
        target = passive;
    }
};

class MoveTo : public ChangeDirection {
public:
    MoveTo(Beast* active, const Beast* passive)
            : ChangeDirection(active, passive) {
        type = chase;
    }

    void handle() override;
};

class MoveFrom : public ChangeDirection {
public:
    MoveFrom(Beast* active, const Beast* passive)
            : ChangeDirection(active, passive) {
        type = run;
    }

    void handle() override;
};

class MarkToDelete : public Event {
public:
    MarkToDelete(Beast* _target, Model* m) : Event(markToDelete, m) {
        pBeast = _target;
    }

    void handle() override;
};

class StopProcessing : public Event {
public:
    explicit StopProcessing(Model* m) : Event(stopProcessing, m) {}

    void handle() override;
};
