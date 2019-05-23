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

struct Color
{
    int R, G, B;
};

struct inside
{
    int number;
    double startTime;
};

class LogViewer
{
    double steptime;
    vector<LogData*> data;
    LogData* massData;
    int id;
    vector <inside>isInside;
    atomic <bool> threadRunning;
    Color color;
public:
    void run();
    void query();
    LogViewer(string file, int ID, LogData* massD);
    ~LogViewer();
    bool isNetThreadRunning() {return threadRunning;}
    int getID() {return id;}
    vector <LogData*> getPastPositions();
    thread *createThreadedQuery();
    void printAllQueriedInfo();
    Color getColor();
    void writeQueriedInfo(string filename);

    void writePastLocations(string filename);
};

#endif