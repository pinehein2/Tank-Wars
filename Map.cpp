#include "Location.h"
#include "Map.h"

// creates a grid of empty cells with the given number of rows and columns
Map::Map(int rows, int cols){
    this->rows = rows;
    this->cols = cols;
    this->map = new Location [rows*cols];
    this->zone = 0;
}
// frees any bullets still on the board and the grid of cells itself
Map::~Map() {
    // only bullets get deleted here, tanks are freed by their own controller instead, so we skip them to avoid deleting the same thing twice
    for (int i = 0; i < rows*cols; i++) {
        Entity* e = map[i].getEntity();
        if (e != nullptr && e->getTag() == Tag::BULLET) {
            delete e;
        }
    }
    delete[] map;
}
// returns how many rows the board has
int Map::getRows() {
    return rows;
}
// returns how many columns the board has
int Map::getCols() {
    return cols;
}
// shrinks the safe zone inward by one more ring of cells
void Map::decreaseZone() {
    this->zone++;
    // a cell becomes inactive once it is within "zone" rings of any edge, so the safe area shrinks toward the center
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if ((i < zone || i > (rows - zone - 1)) || (j < zone || j > (cols - zone - 1))){
                this->getLocation(i, j)->deactivate();
            }
        }
    }
}
// returns the cell at a given spot, or null if that spot is outside the board
Location* Map::getLocation(int x, int y){
    if (x >= 0 && y >= 0 && x < rows && y < cols) {
        return &map[(x*this->cols)+y];
    } else {
        return nullptr;
    }
}
