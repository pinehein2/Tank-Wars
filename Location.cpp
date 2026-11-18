#include "Entity.h"
#include "Location.h"

// creates an empty, active, unselected cell
Location::Location(){
    this->entity = nullptr;
    this->active = true;
    this->selectable = false;
}
// returns whatever entity is currently standing on this cell, or null if empty
Entity* Location::getEntity(){
    return entity;
}
// returns whether this cell is currently empty
bool Location::canEnter(){
    return entity == nullptr;
}
// places an entity onto this cell
void Location::enterLoc(Entity *entity){
    this->entity = entity;
}
// removes whatever entity is on this cell
void Location::exitLoc(){
    this->entity = nullptr;
}
// marks this cell as one of the currently offered move options
void Location::select(){
    this->selectable = true;
}
// unmarks this cell as a currently offered move option
void Location::deselect() {
    this->selectable = false;
}
// marks this cell as outside the current safe zone
void Location::deactivate() {
    this->active = false;
}
// returns whether this cell is still inside the current safe zone
bool Location::isActive() {
    return active;
}
// returns the character that should be drawn for this cell on the map
char Location::getIcon(){
    // checks in order of priority, so a tank/bullet icon always wins over showing this cell as selectable or shrunk
    if (entity != nullptr) {
        return entity->getIcon();
    } else if (selectable) {
        return 'O';
    } else if (active) {
        return ' ';
    } else {
        return '-';
    }
}
