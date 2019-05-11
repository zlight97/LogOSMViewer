#ifndef LOGVIEWER_H
#define LOGVIEWER_H
#include <Definitions.h>
#include <string>
#include <vector>
#include <thread>
using namespace std;
struct LogData
{
    double time, nLat, nLong, nE, gpsLat, gpsLong, gpsE;
};

class LogViewer
{
    double steptime;
    vector<LogData*> data;
    thread viewerThread;
public:
    void run();
    void query();
    LogViewer(string file);
    ~LogViewer();
};

#endif