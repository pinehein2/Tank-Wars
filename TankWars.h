#pragma once
#include "EntityController.h"
#include "Map.h"
#include "PrintUI.h"
#include "Util.h"
#include "CrashAction.h"

class TankWars {
    private:
        TankController* p1;
        TankController* p2;
        BulletController b;
        PrintUI *printer;
        Map* map;
        int turn;
        int cols;
        int rows;

        static Direction parseDirection(const string &dir);
        // avoidX and avoidY are only used when avoidOccupied is true, and they mean do not pick this exact spot
        void randomSpawn(int &x, int &y, bool avoidOccupied, int avoidX, int avoidY);
        void promptSpawn(const string &label, int &x, int &y, Direction &direction, bool avoidOccupied, int avoidX, int avoidY);
    public:
        TankWars(GameMode mode, int hp, string filename, int rows = 20, int cols = 20);
        ~TankWars();
        int judge();
        int playTurn();
};
