#pragma once

#include "Map.h"
#include "Util.h"
#include "EntityController.h"
#include "MoveAction.h"
#include <fstream>
#include <string>
using namespace std;

class PrintUI{
    public:
        PrintUI(string filename);

        ~PrintUI();

        void printMap(Map *map);

        void printDivider(int n);

        void printDividerLog(int n);

        void printPlayer(TankController* tank);

        void printActionList(TankController* tank);

        void printAnnouncement(string str);

        void printAnnouncementLog(string str);

        void inputLog(string str);
    private:
        ofstream log;
};



