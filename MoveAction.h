#pragma once

#include <string>
#include "Map.h"
#include "Entity.h"
using namespace std;

// this is just a shared base so MoveAction and CrashAction can both describe what they did in the same way
class Action {
    public:
        virtual ~Action() {}
        virtual string getDescription() = 0;
};

class MoveAction : public Action {
    public:
        MoveAction();
        MoveAction(int x, int y);
        void setX(int x);
        void setY(int y);
        void setDirection(Direction direction);
        int getX();
        int getY();
        string act(Map *map, Entity *entity);
        string getDescription() override;
    private:
        int x, y;
        Direction direction;
};