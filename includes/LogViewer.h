#ifndef LOGVIEWER_H
#define LOGVIEWER_H
#include <Definitions.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <POI.h>
using namespace std;
struct LogData
{
    double time, nLat, nLong, nE, gpsLat, gpsLong, gpsE;
    vector <POI*> pois;
};

class LogViewer
{
    double steptime;
    vector<LogData*> data;
    int id;
    atomic <bool> threadRunning;
public:
    void run();
    void query();
    LogViewer(string file, int ID);
    ~LogViewer();
    bool isNetThreadRunning() {return threadRunning;}
    int getID() {return id;}
    vector <LogData*> getPastPositions();
};

#endif