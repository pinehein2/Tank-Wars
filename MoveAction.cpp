#include "Map.h"
#include "Entity.h"
#include "MoveAction.h"
#include <string>
using namespace std;

// creates a move action pointing at the top-left corner by default
MoveAction::MoveAction() {
    this->x = 0;
    this->y = 0;
}
// creates a move action pointing at a given spot
MoveAction::MoveAction(int x, int y) {
    this->x = x;
    this->y = y;
}
// sets the row this move action points to
void MoveAction::setX(int x) {
    this->x = x;
}
// sets the column this move action points to
void MoveAction::setY(int y) {
    this->y = y;
}
// sets the facing direction this move action results in
void MoveAction::setDirection(Direction direction) {
    this->direction = direction;
}
// returns the row this move action points to
int MoveAction::getX() {
    return x;
}
// returns the column this move action points to
int MoveAction::getY() {
    return y;
}
// actually moves the given entity to this move action's spot and direction, and returns a message describing it
string MoveAction::act(Map *map, Entity *entity) {
    // tanks always stay on the board now, so these null checks are just a safety net
    Location* oldLoc = map->getLocation(entity->getX(), entity->getY());
    // if the other tank already moved onto this cell this turn, this cell no longer belongs to us, so leave it alone
    if (oldLoc != nullptr && oldLoc->getEntity() == entity) oldLoc->exitLoc();
    Location* newLoc = map->getLocation(x, y);
    if (newLoc != nullptr) newLoc->enterLoc(entity);
    entity->updateDirection(direction);
    entity->updatePosition(x, y);
    return entity->getName() + " has moved to (" + to_string(x+1) + ", " + to_string(y+1) + ").";
}
// builds the short line shown in the move menu describing where this move goes
string MoveAction::getDescription() {
    return "Move to (" + to_string(x+1) + ", " + to_string(y+1) + ").";
}
