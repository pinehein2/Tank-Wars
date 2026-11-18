#pragma once

enum Direction {
    NORTH, EAST, SOUTH, WEST
};

enum Tag {
    TANK, BULLET
};

enum GameMode {
    PVP, PVE, DEMO
};

enum CMDOptions{
    LOG = 1000, // this number just needs to not clash with any letter used for the other options
};