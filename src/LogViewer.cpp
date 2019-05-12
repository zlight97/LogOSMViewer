#include <LogViewer.h>
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>
#include <json.hpp>
#include <unistd.h>
using namespace std;

LogViewer::LogViewer(string file, int ID)
{
    id = ID;
    fstream f;
    f.open(file, ios::in);
    string line, word, temp;
    vector<string> v;
    while (getline(f,line))
    {
        v.clear();
        stringstream st(line);

        while(getline(st,word,','))
        {
            v.push_back(word);
        }

        LogData *d = new LogData;
        // for(string s :v)
        //     cout<<s<<endl;
        d->time = stod(v[0]);
        d->nLat = stod(v[1]);
        d->nLong = stod(v[2]);
        d->nE = stod(v[3]);
        d->gpsLat = stod(v[4]);
        d->gpsLong = stod(v[5]);
        d->gpsE = stod(v[6]);
        data.push_back(d);
        
    }
    if(data.size()>1)
        steptime = data[1]->time-data[0]->time;
}

LogViewer::~LogViewer()
{
    for(int i = 0; i <data.size(); i++)
        delete data[i];
    data.clear();
}

void LogViewer::run()
{
    while(1)
    {
        while(paused)
            usleep(resolution);
        double index = (current_time/steptime) - data[0]->time/steptime;
        int i = (int) index;
        if(i>=0&&i<data.size())
        {
            cout<<"ID: "<<id<<" TIME: "<<data[i]->time<<"\nnLat: "<<data[i]->nLat<<" nLong: "<<data[i]->nLong<<" nElev: "<<data[i]->nE<<endl;
            cout<<"gpsLat: "<<data[i]->gpsLat<<" gpsLong: "<<data[i]->gpsLong<<" gpsElev: "<<data[i]->gpsE<<endl;
        }
        if(endd)
            return;
        usleep(resolution);
    }
}

void LogViewer::query()
{

}