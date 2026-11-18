#pragma once

#include "Location.h"

class Map{
    public:
        Map(int rows, int cols);
        ~Map();
        int getRows();
        int getCols();
        void decreaseZone();
        Location* getLocation(int x, int y);

    private:
        int rows;
        int cols;
        int zone;
        Location *map;
};