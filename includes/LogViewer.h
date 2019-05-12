#ifndef LOGVIEWER_H
#define LOGVIEWER_H
#include <Definitions.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <POI.h>
using namespace std;
struct LogData
{
    double time, nLat, nLong, nE, gpsLat, gpsLong, gpsE;
    vector <POI*> pois;
};

class LogViewer
{
    static mutex mtx;
    double steptime;
    vector<LogData*> data;
    thread viewerThread;
    int id;
public:
    void run();
    void query();
    LogViewer(string file, int ID);
    ~LogViewer();
};

#endif