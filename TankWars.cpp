#include "EntityController.h"
#include "Map.h"
#include "PrintUI.h"
#include "Util.h"
#include "TankWars.h"
#include <iostream>
#include <cstdlib>

// turns a direction word typed by the user into the matching Direction value
Direction TankWars::parseDirection(const string &dir) {
    if (dir == "NORTH") return Direction::NORTH;
    if (dir == "EAST") return Direction::EAST;
    if (dir == "SOUTH") return Direction::SOUTH;
    return Direction::WEST;
}

// picks a random spot on the board for an ai tank to spawn at, avoiding the other tank's spot if asked to
void TankWars::randomSpawn(int &x, int &y, bool avoidOccupied, int avoidX, int avoidY) {
    // keeps picking a random spot until it finds one that is not the other tank's spot
    do {
        x = rand() % rows;
        y = rand() % cols;
    } while (avoidOccupied && x == avoidX && y == avoidY);
}

// asks a human player for a spawn spot and direction, and keeps re-asking until the answer is valid
void TankWars::promptSpawn(const string &label, int &x, int &y, Direction &direction, bool avoidOccupied, int avoidX, int avoidY) {
    string dir;
    while (true) {
        printer->printAnnouncement("Please enter the initial position of " + label + " Example:'(1-20) (1-20) (NORTH, EAST, SOUTH, WEST)':");
        if (!(cin >> x >> y >> dir)) {
            cin.clear();
            cin.ignore(10000, '\n');
            printer->printAnnouncement("Invalid input. Please try again.");
            continue;
        }
        if (x < 1 || x > rows || y < 1 || y > cols) {
            printer->printAnnouncement("Position out of range. Please try again.");
            continue;
        }
        if (dir != "NORTH" && dir != "EAST" && dir != "SOUTH" && dir != "WEST") {
            printer->printAnnouncement("Invalid direction. Please try again.");
            continue;
        }
        if (avoidOccupied && x - 1 == avoidX && y - 1 == avoidY) {
            printer->printAnnouncement("That position is already taken by the other tank. Please choose another position.");
            continue;
        }
        break;
    }
    direction = parseDirection(dir);
    x--;
    y--;
    printer->inputLog(label + " Spawn: " + to_string(x) + ", " + to_string(y) + " Direction: " + dir);
}

// sets up a new game: creates the board and both tanks, spawning them either from player input or automatically for ai
TankWars::TankWars(GameMode mode, int hp, string filename, int rows, int cols) {
    this->turn = 1;
    this->cols = cols;
    this->rows = rows;
    this->printer = new PrintUI(filename);
    this->map = new Map(rows, cols);
    string gm = (mode == 0 ? "PVP" : (mode == 1 ? "PVE" : "DEMO"));
    this->b = BulletController();
    printer->printAnnouncement("You have chosen the " + gm + " mode!");

    int x1, y1;
    Direction direction1;
    if (mode == GameMode::DEMO) {
        randomSpawn(x1, y1, false, 0, 0);
        direction1 = Direction(rand() % 4);
        printer->printAnnouncementLog("CPU A automatically spawned at (" + to_string(x1+1) + ", " + to_string(y1+1) + ").");
    } else {
        promptSpawn("Player A", x1, y1, direction1, false, 0, 0);
    }

    if (mode == GameMode::DEMO) {
        this->p1 = new AITankController("CPU A", x1, y1, direction1, hp, 'A');
    } else {
        this->p1 = new PlayerTankController("Player A", x1, y1, direction1, hp, 'A');
    }
    map->getLocation(x1, y1)->enterLoc(p1->getEntity());

    int x2, y2;
    Direction direction2;
    if (mode == GameMode::PVE || mode == GameMode::DEMO) {
        randomSpawn(x2, y2, true, x1, y1);
        direction2 = Direction(rand() % 4);
        printer->printAnnouncementLog(string("CPU B") + " automatically spawned at (" + to_string(x2+1) + ", " + to_string(y2+1) + ").");
    } else {
        promptSpawn("Player B", x2, y2, direction2, true, x1, y1);
    }

    if (mode == GameMode::PVE || mode == GameMode::DEMO) {
        this->p2 = new AITankController("CPU B", x2, y2, direction2, hp, 'B');
    } else {
        this->p2 = new PlayerTankController("Player B", x2, y2, direction2, hp, 'B');
    }
    map->getLocation(x2, y2)->enterLoc(p2->getEntity());
}

// cleans up everything this game allocated when it is done
TankWars::~TankWars() {
    // map has to be deleted before the tanks, otherwise it would be looking at tank memory that is already gone
    delete map;
    delete p1;
    delete p2;
    delete printer;
}

// checks if either tank has run out of health and announces the winner, draw, or that the game should keep going
int TankWars::judge() {
    if (p1->getHealth() > 0 && p2->getHealth() > 0) {
        return 0;
    } else if (p1->getHealth() <= 0 && p2->getHealth() > 0) {
        printer->printAnnouncementLog(p2->getName() + " has won!");
    } else if (p2->getHealth() <= 0 && p1->getHealth() > 0) {
        printer->printAnnouncementLog(p1->getName() + " has won!");
    } else if (p1->getHealth() <= 0 && p2->getHealth() <= 0) {
        printer->printAnnouncementLog("Draw!");
    }
    printer->printAnnouncementLog("Game End");
    printer->printDividerLog((cols+1)*4);
    return 1;
}

// runs one full turn of the game: both tanks move and shoot, then bullets move and the zone may shrink
int TankWars::playTurn() {
    // the order for each turn is player 1 moves and shoots, then player 2 moves and shoots, then bullets move, then the zone can shrink
    p1->activate();
    p1->updateActions(map);

    printer->printDividerLog((cols+1)*4);
    printer->printAnnouncementLog("Turn " + to_string(turn));

    printer->printMap(map);
    printer->printDivider((cols+1)*4);
    printer->printPlayer(p1);
    printer->printPlayer(p2);

    printer->printDivider((cols+1)*4);

    printer->printActionList(p1);
    if (p1->getNumOfActions() > 0) {
        printer->printAnnouncementLog(p1->getInput(map)->act(map, p1->getEntity()));
    } else {
        printer->printAnnouncementLog(p1->getName() + " has no valid moves.");
    }

    // checking right here, before player 1 even gets to shoot, means player 2 never gets a turn this round if a crash already happened
    CrashAction crash;
    if (crash.resolve(p1, p2)) {
        printer->printAnnouncementLog(crash.getDescription());
        printer->printAnnouncementLog("Game End");
        printer->printDividerLog((cols+1)*4);
        p1->clearActions(map);
        p1->deactivate();
        this->turn++;
        return 1;
    }

    string p1ShootMsg = p1->shoot(map, turn);
    if (p1ShootMsg.length() > 0) printer->printAnnouncementLog(p1ShootMsg);

    p1->clearActions(map);
    p1->deactivate();
    printer->printDivider((cols+1)*4);
    p2->activate();
    p2->updateActions(map);

    printer->printMap(map);
    printer->printPlayer(p1);
    printer->printPlayer(p2);

    printer->printDivider((cols+1)*4);

    printer->printActionList(p2);
    if (p2->getNumOfActions() > 0) {
        printer->printAnnouncementLog(p2->getInput(map)->act(map, p2->getEntity()));
    } else {
        printer->printAnnouncementLog(p2->getName() + " has no valid moves.");
    }

    if (crash.resolve(p1, p2)) {
        printer->printAnnouncementLog(crash.getDescription());
        printer->printAnnouncementLog("Game End");
        printer->printDividerLog((cols+1)*4);
        p2->clearActions(map);
        p2->deactivate();
        this->turn++;
        return 1;
    }

    string p2ShootMsg = p2->shoot(map, turn);
    if (p2ShootMsg.length() > 0) printer->printAnnouncementLog(p2ShootMsg);

    printer->printDivider((cols+1)*4);

    p2->clearActions(map);
    p2->deactivate();
    //----------------------------------------------------
    // bullets move here, after both tanks moved and fired, so a bullet fired this turn still gets to move this same turn
    string msg = b.tick(map);
    b.resetMoved(map);
    if (msg.length() > 0) printer->printAnnouncementLog(msg);

    // the play area shrinks by one ring every 16 turns
    if (turn % 16 == 0) {
        printer->printAnnouncementLog("The zone is shrinking!");
        map->decreaseZone();
    }

    msg = p1->checkZone(map);
    if (msg.length() > 0) printer->printAnnouncementLog(msg);
    msg = p2->checkZone(map);
    if (msg.length() > 0) printer->printAnnouncementLog(msg);

    this->turn++;
    //----------------------------------------------------
    return this->judge();
}
