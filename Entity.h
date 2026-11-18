#pragma once

#include <string>
#include "Util.h"
using namespace std;

class Entity {
    public:
        Tag getTag();
        void setTag(Tag tag);

        Direction getDirection();
        void updateDirection(Direction direction);

        int getX();
        int getY();
        void updatePosition(int x, int y);

        int getHealth();
        void setHealth(int health);

        string getName();
        void setName(string name);
        
        int getDmg();
        string hurt(int dmg, string name);

        char getIcon();

        // returns whether a bullet has already moved during this turn's bullet pass
        bool hasMoved() {
            return moved;
        }
        // this just tracks whether a bullet already moved this turn so it does not move twice
        void toggleMoved() {
            moved = !moved;
        }
    protected:
        int x, y, health, dmg;
        string name;
        Direction direction;
        Tag tag;
        char icon;
        bool moved;
};

class Bullet : public Entity {
    public:
        Bullet(int x, int y, Direction direction, int dmg = 2);
};


class Tank : public Entity {
    public:
        Tank();
        Tank(int x, int y, string name, Direction direction, char icon, int health = 5);
        string getStr();
};