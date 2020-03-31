//
// Created by radko on 30.09.16.
//

#pragma once

enum Species {
    unidentified,
    dragonfly,
    stork,
    hedgehog,
    frog
};

enum State {
    hanging,
    chasing,
    running
};

class Beast {
public:
    Beast();
    Beast(int _id, long long _x, long long _y, long long _z, long long _vx, long long _vy, long long _vz, int _sens);

    void move();

    virtual bool isAfraidOf(const Beast* _target) const = 0;
    virtual bool isFood(const Beast* _target) const = 0;

    bool isInteractWith(const Beast *_target) const;
    bool canSense(const Beast* _target) const;
    bool canEat(const Beast* _target) const;
    void changeDirectionTo(const Beast* _target);
    void changeDirectionFrom(const Beast* _target);

    double getDistance(const Beast* _target) const;
    Species getSpecies() const { return species; }

    long long getX() const { return x; }
    long long getY() const { return y; }
    long long getZ() const { return z; }
    long long getVX() const { return vx; }
    long long getVY() const { return vy; }
    long long getVZ() const { return vz; }

    const Beast* getTarget() const { return target; }

    State getState() const { return state; }
    void updateState();
    void toNullState();
protected:
    void projectSpeedOntoOZ();
//Data
    int id;
    int sensitivity;
    int eatRange = 1;
    long long x, y, z, vx, vy, vz;
    double speed;
    Species species;
    State state;
    const Beast* target = nullptr;
};


class Hedgehog : public Beast {
public:
    Hedgehog() : Beast() {
        species = hedgehog;
    }
    Hedgehog(int _id, long long _x, long long _y, long long _vx, long long _vy, int _sens)
            : Beast(_id, _x, _y, 0, _vx, _vy, 0, _sens) {
        species = hedgehog;
    }

    bool isAfraidOf(const Beast* target) const override;
    bool isFood(const Beast* target) const override;
};

class Frog : public Beast {
public:
    Frog() : Beast() {
        species = frog;
    }
    Frog(int _id, long long _x, long long _y, long long _vx, long long _vy, int _sens)
            : Beast(_id, _x, _y, 0, _vx, _vy, 0, _sens) {
        species = frog;
        eatRange = 3;
    }

    bool isAfraidOf(const Beast* target) const override;
    bool isFood(const Beast* target) const override;
};

class Dragonfly : public Beast {
public:
    Dragonfly() : Beast() {
        species = dragonfly;
    }
    Dragonfly(int _id, long long _x, long long _y, long long _z, long long _vx, long long _vy, long long _vz, int _sens)
            : Beast(_id, _x, _y, _z, _vx, _vy, _vz, _sens) {
        species = dragonfly;
    }

    bool isAfraidOf(const Beast* target) const override;
    bool isFood(const Beast* target) const override;
};

class Stork : public Beast {
public:
    Stork() : Beast() {
        species = stork;
    }
    Stork(int _id, long long _x, long long _y, long long _z, long long _vx, long long _vy, long long _vz, int _sens)
            : Beast(_id, _x, _y, _z, _vx, _vy, _vz, _sens) {
        species = stork;
    }

    bool isAfraidOf(const Beast* target) const override;
    bool isFood(const Beast* target) const override;
};
