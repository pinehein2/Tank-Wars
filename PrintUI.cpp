#include "Map.h"
#include "Util.h"
#include "EntityController.h"
#include "MoveAction.h"
#include "PrintUI.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
using namespace std;

// opens the log file so game events can be written to it as the game runs
PrintUI::PrintUI(string filename) {
    this->log = ofstream(filename, ios::app);
}

// closes the log file when the game is done
PrintUI::~PrintUI() {
    log.close();
}

// prints the whole board to the screen, with column numbers along the top and row numbers down the side
void PrintUI::printMap(Map *map) {
    // this whole function is just lining up columns nicely with spacing, it prints the header row of column numbers first
    cout << setw(4) << setfill(' ') << "|";
    for(int j = 0; j < map->getCols(); j++) {
        cout << setw(4) << setfill(' ') << to_string(j+1) + "|";
    }
    cout << endl;
    for(int i = 0; i < map->getRows(); i++) {
        cout << setw(3) << right << to_string(i+1) << "| ";
        for(int j = 0; j < map->getCols(); j++) {
            cout << map->getLocation(i, j)->getIcon() << " | ";
        }
        cout << endl;
    }
}

// prints a line of equals signs to the screen to separate sections
void PrintUI::printDivider(int n) {
    cout << setfill('=') << setw(n) << "\n";
}

// prints a line of equals signs to both the screen and the log file
void PrintUI::printDividerLog(int n) {
    cout << setfill('=') << setw(n) << "\n";
    log << setfill('=') << setw(n) << "\n";
}

// prints a tank's status line, with an arrow marker if it is currently that tank's turn
void PrintUI::printPlayer(TankController* tank) {
    cout << (tank->isActive() ? ">>" : "  ") << tank->getTankStatus() << endl;
}

// prints the numbered list of move options currently available to a tank
void PrintUI::printActionList(TankController* tank) {
    MoveAction* actions = tank->getActions();
    cout << tank->getName() << "'s turn!" << endl;
    for (int i = 0; i < tank->getNumOfActions(); i++) {
        cout << to_string(i+1) << "]- " << actions[i].getDescription() << endl;
    }
}

// prints a message to the screen only
void PrintUI::printAnnouncement(string str) {
    cout << str << endl;
}

// prints a message to both the screen and the log file
void PrintUI::printAnnouncementLog(string str) {
    cout << str << endl;
    log << str << endl;
}

// writes a message to the log file only, without showing it on screen
void PrintUI::inputLog(string str) {
    log << str << endl;
}
