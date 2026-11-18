#pragma once

#include "Map.h"
#include "Util.h"
#include "MoveAction.h"
#include "Entity.h"
#include <string>
using namespace std;

class TankController {
    public:
        // this needs to be virtual so deleting through a TankController pointer still cleans up the actual player or ai controller correctly
        virtual ~TankController() {
            delete[] actions;
            delete entity;
        }
        string shoot(Map *map, int turn);
        string checkZone(Map *map);
        void updateActions(Map *map);

        void clearActions(Map *map);

        MoveAction* getActions();

        bool isActive();

        void activate();

        void deactivate();

        string getTankStatus();

        char getTankIcon();

        string getName();

        Entity* getEntity();

        int getHealth();

        int getNumOfActions();

        virtual MoveAction* getInput(Map *map) = 0;
    protected:
        Tank* entity;
        bool active;
        MoveAction* actions;
        int num_of_actions;
};

class PlayerTankController : public TankController {   
    public:
        PlayerTankController(string name, int x, int y, Direction direction, int health, char icon);
        MoveAction* getInput(Map *map) override;
};

class AITankController : public TankController {
    public:
        AITankController(string name, int x, int y, Direction direction, int health, char icon);
        MoveAction* getInput(Map *map) override;
};

class BulletController {
    public:
        string tick(Map *map);
        void resetMoved(Map *map);
};