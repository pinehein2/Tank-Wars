#include "CrashAction.h"

// checks if two tanks are standing on the same cell, and if so works out who wins based on health
bool CrashAction::resolve(TankController *a, TankController *b) {
    if (a->getEntity()->getX() != b->getEntity()->getX() ||
        a->getEntity()->getY() != b->getEntity()->getY()) {
        return false;
    }
    if (a->getHealth() > b->getHealth()) {
        description = a->getName() + " crashed into " + b->getName() + " and won!";
    } else if (b->getHealth() > a->getHealth()) {
        description = b->getName() + " crashed into " + a->getName() + " and won!";
    } else {
        description = a->getName() + " and " + b->getName() + " crashed into each other. Draw!";
    }
    return true;
}

// returns the message describing the outcome of the last resolved crash
string CrashAction::getDescription() {
    return description;
}
