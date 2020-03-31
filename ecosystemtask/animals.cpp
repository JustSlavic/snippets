//
// Created by radko on 10/1/16.
//

#include <cmath>
#include "animals.h"

#define epsilon 0.0001


bool Beast::canSense(const Beast* _target) const {
    return getDistance(_target) <= sensitivity;
}

bool Beast::canEat(const Beast* _target) const {
    return getDistance(_target) <= eatRange;
}

double Beast::getDistance(const Beast* _target) const {
    return sqrt((x - _target->x)*(x - _target->x) + (y - _target->y)*(y - _target->y) + (z - _target->z)*(z - _target->z));
}

void Beast::changeDirectionTo(const Beast* _target) {
    double&& __x = _target->x - x;
    double&& __y = _target->y - y;
    double&& __z = _target->z - z;

    double&& __distance = getDistance(_target);
    if (__distance == 0) return;

    double&& __newVx = __x * speed / __distance;
    double&& __newVy = __y * speed / __distance;
    double&& __newVz = __z * speed / __distance;

    if (fabs(__newVx - round(__newVx)) < epsilon) {
        vx = (long long) round(__newVx);
    } else {
        vx = (long long) trunc(__newVx);
    }

    if (fabs(__newVy - round(__newVy)) < epsilon) {
        vy = (long long) round(__newVy);
    } else {
        vy = (long long) trunc(__newVy);
    }

    if (fabs(__newVz - round(__newVz)) < epsilon) {
        vz = (long long) round(__newVz);
    } else {
        vz = (long long) trunc(__newVz);
    }

    if (z == 0 && vz != 0 && (species == frog || species == hedgehog)) {
        double&& __projectionLength = sqrt(__newVx*__newVx + __newVy*__newVy);
        if (__projectionLength == 0) {
            vx = vy = vz = 0;
            return;
        }
        double&& _x = __newVx * speed / __projectionLength;
        double&& _y = __newVy * speed / __projectionLength;

        if (fabs(_x - round(_x)) < epsilon) {
            vx = (long long) round(_x);
        } else {
            vx = (long long) trunc(_x);
        }

        if (fabs(_y - round(_y)) < epsilon) {
            vy = (long long) round(_y);
        } else {
            vy = (long long) trunc(_y);
        }
        vz = 0;
    }

    state = chasing;
    target = _target;
}

void Beast::changeDirectionFrom(const Beast* _target) {
    changeDirectionTo(_target);

    vx = -vx;
    vy = -vy;
    vz = -vz;

    state = running;
}


Beast::Beast(int _id, long long _x, long long _y, long long _z, long long _vx, long long _vy, long long _vz, int _sens)
    : x(_x), y(_y), z(_z), vx(_vx), vy(_vy), vz(_vz), sensitivity(_sens), id(_id) {
    speed = sqrt(vx*vx + vy*vy + vz*vz);
}

void Beast::move() {
    if (z+vz < 0) { //if move under ground
        x = x - vx*z/vz;
        y = y - vy*z/vz;
        z = 0;
    } else { //common move
        x += vx;
        y += vy;
        z += vz;
    }
    //if touches the ground from the sky or trying to jump from the ground
    if (z == 0 && vz < 0 || z == 0 && vz > 0 && (species == frog || species == hedgehog)) {
        projectSpeedOntoOZ();
    }
}

void Beast::updateState() {
    if (target == 0 || getDistance(target) > sensitivity) {
        state = hanging;
    }
}

void Beast::projectSpeedOntoOZ() {
    double __projectionLength = sqrt(vx*vx + vy*vy);
    if (__projectionLength == 0) {
        vx = vy = vz = 0;
        return;
    }
    double __newVx = vx * speed / __projectionLength;
    double __newVy = vy * speed / __projectionLength;
    vx = (long long) trunc(__newVx);
    vy = (long long) trunc(__newVy);
    vz = 0;
}

bool Beast::isInteractWith(const Beast* _target) const {
    return target == _target;
}

void Beast::toNullState() {
    target = nullptr;
}

Beast::Beast(): id(0), x(0), y(0), vx(0), vy(0), z(0), vz(0),
                sensitivity(0), speed(0), species(unidentified),
                state(hanging) {}

bool Hedgehog::isAfraidOf(const Beast* target) const {
    return false;
}

bool Hedgehog::isFood(const Beast* target) const {
    return target->getSpecies() == frog;
}


bool Frog::isAfraidOf(const Beast* target) const {
    return target->getSpecies() == stork || target->getSpecies() == hedgehog;
}

bool Frog::isFood(const Beast* target) const {
    return target->getSpecies() == dragonfly;
}


bool Dragonfly::isAfraidOf(const Beast* target) const {
    return target->getSpecies() == stork || target->getSpecies() == frog;
}

bool Dragonfly::isFood(const Beast* target) const {
    return false;
}


bool Stork::isAfraidOf(const Beast* target) const {
    return false;
}

bool Stork::isFood(const Beast* target) const {
    return target->getSpecies() == dragonfly || target->getSpecies() == frog;
}
