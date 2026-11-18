#include "Map.h"
#include "Util.h"
#include "MoveAction.h"
#include "Entity.h"
#include "EntityController.h"
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <array>
using namespace std;

// these two arrays convert a Direction (NORTH, EAST, SOUTH, WEST) into a one-step move on the grid
int dx[] = {-1,0,1,0};
int dy[] = {0,1,0,-1};

// fires a bullet from this tank if it is the tank's turn to shoot, and returns a message if it hits something right away
string TankController::shoot(Map *map, int turn) {
    string msg = "";
    // a tank fires on turn 1, then every 3rd turn after that
    if ((turn-1) % 3 == 0) {
        Bullet *b = new Bullet(entity->getX() + dx[entity->getDirection()]*2, entity->getY() + dy[entity->getDirection()]*2, entity->getDirection());
        Location *l = map->getLocation(b->getX(), b->getY());
        if (l == nullptr) {
            // the bullet would spawn off the board, so it just never exists
            delete b;
        } else if (l->canEnter()) {
            l->enterLoc(b);
        } else {
            Entity *e = l->getEntity();
            if (e->getTag() == Tag::TANK) {
                msg = e->hurt(b->getDmg(), entity->getName());
            } else if (e->getTag() == Tag::BULLET) {
                l->exitLoc();
                delete e;
            }
            delete b;
        }
    }
    return msg;
}

// checks if this tank is standing outside the current safe zone and hurts it if so
string TankController::checkZone(Map *map) {
    Location* l = map->getLocation(entity->getX(), entity->getY());
    if (l != nullptr && !l->isActive()) {
        return entity->hurt(1, "The Zone");
    }
    return "";
}

// works out the three moves this tank is allowed to make this turn
void TankController::updateActions(Map *map) {
    int ind = 0;
    // this checks the three moves a tank can make each turn, turning left, going straight, or turning right
    for (int i = entity->getDirection() - 1; i <= entity->getDirection() + 1; i++) {
        int d = i;
        if (i < 0) {
            d = 3;
        } else if (i > 3) {
            d = 0;
        }
        int x = entity->getX()+dx[d];
        int y = entity->getY()+dy[d];
        Location* l = map->getLocation(x,y);
        // l is null when a spot is off the board, and tanks are never allowed to leave the board
        // a cell holding the other tank is still allowed here, on purpose, so two tanks can crash into each other
        bool occupiedByTank = l != nullptr && l->getEntity() != nullptr && l->getEntity()->getTag() == Tag::TANK;
        if (l != nullptr && (l->canEnter() || occupiedByTank)) {
            l->select();
            actions[ind].setX(x);
            actions[ind].setY(y);
            actions[ind].setDirection(Direction(d));
            ind++;
        }
    }
    this->num_of_actions = ind;
}

// unmarks the cells that were shown as selectable move options for this tank
void TankController::clearActions(Map *map) {
    for (int i = 0; i < num_of_actions; i++) {
        Location* l = map->getLocation(actions[i].getX(), actions[i].getY());
        if (l != nullptr) l->deselect();
    }
}

// returns the list of this tank's current move options
MoveAction* TankController::getActions() {
    return actions;
}

// returns how many move options this tank currently has
int TankController::getNumOfActions() {
    return num_of_actions;
}

// returns whether it is currently this tank's turn
bool TankController::isActive() {
    return active;
}

// marks this tank as the one whose turn it currently is
void TankController::activate() {
    active = true;
}

// marks this tank as no longer having its turn
void TankController::deactivate() {
    active = false;
}

// returns the status line for this tank, like its icon, name, and health
string TankController::getTankStatus() {
    return entity->getStr();
}

// returns the single character used to draw this tank on the map
char TankController::getTankIcon() {
    return entity->getIcon();
}

// returns this tank's display name
string TankController::getName() {
    return entity->getName();
}

// returns the underlying tank entity this controller is managing
Entity* TankController::getEntity() {
    return entity;
}

// returns this tank's current health
int TankController::getHealth() {
    return entity->getHealth();
}

// creates a human-controlled tank at a chosen spot, facing direction, health, and icon
PlayerTankController::PlayerTankController(string name, int x, int y, Direction direction, int health, char icon) {
    this->num_of_actions = 0;
    this->entity = new Tank(x, y, name, direction, icon, health);
    this->active = false;
    this->actions = new MoveAction[3];
}
// reads a move choice typed by the human player, and keeps asking until it is a valid option number
MoveAction* PlayerTankController::getInput(Map *map) {
    int x;
    do {
        if (!(cin >> x)) {
            cin.clear();
            cin.ignore(10000, '\n');
        }
    } while (x < 1 || x > num_of_actions);
    
    return &actions[x-1];
}

// creates an ai-controlled tank at a chosen spot, facing direction, health, and icon
AITankController::AITankController(string name, int x, int y, Direction direction, int health, char icon) {
    this->num_of_actions = 0;
    this->entity = new Tank(x, y, name, direction, icon, health);
    this->active = false;
    this->actions = new MoveAction[3];
}
// picks a random move out of this tank's current legal options
MoveAction* AITankController::getInput(Map *map) {
    return &actions[rand() % num_of_actions];
}

// moves every bullet on the board two cells and resolves any hits, returning a message describing any tanks that got hit
string BulletController::tick(Map *map) {
    string msg = "";
    for (int i = 0; i < map->getRows(); i++) {
        for (int j = 0; j < map->getCols(); j++) {
            Location* l = map->getLocation(i, j);
            Entity* bullet = l->getEntity();
            // skip this cell if there is no bullet here or it already moved earlier in this same pass
            if (bullet == nullptr || bullet->getTag() != Tag::BULLET || bullet->hasMoved()) continue;

            bullet->toggleMoved();
            Direction direction = bullet->getDirection();
            int x = i;
            int y = j;
            // a bullet moves two meters per turn, so this step runs twice
            for (int k = 0; k < 2; k++){
                x += dx[direction];
                y += dy[direction];
                l->exitLoc();
                l = map->getLocation(x, y);
                if (l == nullptr) {
                    delete bullet;
                    break;
                }
                if (l->canEnter()) {
                    l->enterLoc(bullet);
                } else {
                    Entity *e = l->getEntity();
                    if (e->getTag() == Tag::TANK) {
                        if (msg.length() > 0) msg += "\n";
                        msg += e->hurt(bullet->getDmg(), bullet->getName());
                        delete bullet;
                    } else if (e->getTag() == Tag::BULLET) {
                        l->exitLoc();
                        delete e;
                        delete bullet;
                    }
                    break;
                }
            }
        }
    }
    return msg;
}

// resets every bullet's moved flag so they can all move again on the next turn
void BulletController::resetMoved(Map *map) {
    // this flips every bullet's moved flag back to false so they are all free to move again next turn
    for (int i = 0; i < map->getRows(); i++) {
        for (int j = 0; j < map->getCols(); j++) {
            Location* l = map->getLocation(i, j);
            Entity* bullet = l->getEntity();
            if (bullet != nullptr && bullet->getTag() == Tag::BULLET) {
                bullet->toggleMoved();
            }
        }
    }
}
