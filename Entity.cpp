#include <string>
#include "Util.h"
#include "Entity.h"
using namespace std;

// changes which way this entity is facing
void Entity::updateDirection(Direction direction) {
    this->direction = direction;
}
// moves this entity to a new spot on the grid
void Entity::updatePosition(int x, int y) {
    this->x = x;
    this->y = y;
}
// returns whether this entity is a tank or a bullet
Tag Entity::getTag() {
    return tag;
}
// sets whether this entity counts as a tank or a bullet
void Entity::setTag(Tag tag) {
    this->tag = tag;
}
// returns how many life points this entity has left
int Entity::getHealth() {
    return this->health;
}
// sets this entity's life points directly
void Entity::setHealth(int health) {
    this->health = health;
}
// returns this entity's display name
string Entity::getName() {
    return this->name;
}
// changes this entity's display name
void Entity::setName(string name) {
    this->name = name;
}
// returns this entity's row on the grid
int Entity::getX() {
    return x;
}
// returns this entity's column on the grid
int Entity::getY() {
    return y;
}
// returns which way this entity is currently facing
Direction Entity::getDirection() {
    return direction;
}
// takes damage away from this entity's health and returns a message describing it
string Entity::hurt(int dmg, string name) {
    this->health-=dmg;
    return this->name + " was damaged by " + name + " for " + to_string(dmg) + " point(s)!";
}
// returns how much damage this entity deals when it hits something
int Entity::getDmg() {
    return dmg;
}
// returns the character used to draw this entity on the map
char Entity::getIcon() {
    return icon;
}

// creates a bullet at a given spot, facing direction, and damage amount
Bullet::Bullet(int x, int y, Direction direction, int dmg) {
    this->name = "Bullet";
    this->icon = '*';
    this->direction = direction;
    this->x = x;
    this->y = y;
    this->dmg = dmg;
    // a bullet's health being 1 just means it breaks the instant it is hit by anything, it is not related to how much damage it deals
    this->health = 1;
    this->tag = Tag::BULLET;
    this->moved = false;
}

// creates a default tank at the top-left corner facing north with 5 health
Tank::Tank() {
    this->name = "Tank";
    this->x = 0;
    this->y = 0;
    this->direction = Direction(0);
    this->health = 5;
    this->tag = Tag::TANK;
    this->dmg = 0;
    this->moved = false;
}
// creates a tank with a chosen spot, name, facing direction, icon, and health
Tank::Tank(int x, int y, string name, Direction direction, char icon, int health) {
    this->name = name;
    this->icon = icon;
    this->x = x;
    this->y = y;
    this->direction = direction;
    this->health = health;
    this->tag = Tag::TANK;
    this->dmg = 0;
    this->moved = false;
}
// builds the short status line shown for this tank, like its icon, name, and health
string Tank::getStr() {
    return "(" + string(1, icon) + ") Name: " + this->getName() + " Health: " + to_string(this->getHealth());
}
