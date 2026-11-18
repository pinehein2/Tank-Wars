#pragma once

#include "Entity.h"

class Location{
    public:
        Location();
        Entity* getEntity();
        bool canEnter();
        void enterLoc(Entity *entity);
        void exitLoc();
        void select();
        void deselect();
        void deactivate();
        bool isActive();
        char getIcon();

    private:
        Entity *entity;
        bool active;
        bool selectable;

};