#include <iostream>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <ctime>
#include "TankWars.h"
#include "Util.h"
using namespace std;

// parses the command line options, sets up a new game, and runs it turn by turn until it ends
int main(int argc, char *argv[]) {
    int opt; 
    int hp = 5;
    GameMode mode = GameMode::PVP;
    string filename = "tankwar.log";
    struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"mode", required_argument, nullptr, 'm'},
        {"initial-life", required_argument, nullptr, 'p'},
        {"log-file", required_argument, nullptr, LOG},
        {nullptr, 0, nullptr, 0}
    };
    while (true) {
        int index = 0;
        // getopt_long reads one command line option at a time and returns -1 once there are no more left
        opt = getopt_long(argc, argv, "hm:p:", long_options, &index);
        if (opt == -1) break;
        switch (opt){
            case 'h':
                cout << "Usage: ./tankwar [options](optional)\n";
                cout << "Options:\n";
                cout << "  -h | --help                          Print this help message and exit.\n";
                cout << "  --log-file <file>                    Log the game process to a file. (Default: tankwar.log)\n";
                cout << "  -m <mode> | --mode=<mode>            Specify the game mode (PVP/PVE/DEMO). (Default: PVP)\n";
                cout << "  -p <point> | --initial-life=<point>  Specify the initial life points of the tanks. (Default: 5)" << endl;
                return 0;
            case 'm':
                if (string(optarg).compare("PVE") == 0) {
                    mode = GameMode::PVE;
                } else if (string(optarg).compare("DEMO") == 0) {
                    mode = GameMode::DEMO;
                } else if (string(optarg).compare("PVP") == 0) {
                    mode = GameMode::PVP;
                } else {
                    cout << "Invalid Argument." << endl;
                    return 0;
                }
                break;    
            case 'p':
                try {
                    hp = stoi(optarg);
                } catch (invalid_argument&) {
                    cout << "Invalid Argument." << endl;
                    return 0;
                }
                break;
            case LOG:
                filename = optarg;
                if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".log") {
                    filename += ".log";
                }
                break;
            default:
                cout << "Invalid Argument." << endl;
                return 0;
        }
    }
    srand(time(nullptr));

    time_t timestamp = time(NULL);
    ofstream log(filename, ios::app);
    log << "========================= NEW GAME =========================" << endl;
    log << ">> " << ctime(&timestamp) << "GameMode: " << ((mode == 0) ? "PVP" : ((mode == 1) ? "PVE" : "DEMO")) << "\nInitial HP: " << hp << "\n";
    log.close();

    TankWars game(mode, hp, filename);
    // playTurn returns 0 while the game should keep going and 1 once someone has won or drawn
    while (game.playTurn() == 0);

    return 0;
    //g++ -std=c++17 -Wall -Wextra -o tankwars main.cpp Entity.cpp Location.cpp Map.cpp MoveAction.cpp CrashAction.cpp EntityController.cpp PrintUI.cpp TankWars.cpp
}