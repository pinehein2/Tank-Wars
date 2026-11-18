#pragma once

#include <string>
#include "MoveAction.h"
#include "EntityController.h"
using namespace std;

class CrashAction : public Action {
    public:
        // returns true if the two tanks are on the same cell, and works out the winner if so
        bool resolve(TankController *a, TankController *b);
        string getDescription() override;
    private:
        string description;
};
