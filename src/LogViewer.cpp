#include <LogViewer.h>
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>
#include <json.hpp>
#include <unistd.h>
using namespace std;
using json = nlohmann::json;
LogViewer::LogViewer(string file, int ID)
{
    threadRunning = 0;
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
    f.close();
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
        {
            usleep(resolution);
        }
        // if(current_time>200&&current_time<210)
        //     query();
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

void LogViewer::query()//thread the call of this
{
    threadRunning=1;
    double index = (current_time/steptime) - data[0]->time/steptime;
    int i = (double) index;
    if(i>=0&&i<data.size())
    {
        if(data[i]->pois.size()==0)
        {
            string lat = to_string(data[i]->nLat);
            string lon = to_string(data[i]->nLong);
            string E = "10";//to_string(data[i]->nE);
            double latFac = 0.00042076252;
            double lonFac = 0.00205188989;
            string bbox = to_string(data[i]->nLat-latFac)+","+to_string(data[i]->nLong-lonFac)+","+to_string(data[i]->nLat+latFac)+","+to_string(data[i]->nLong+lonFac);
            // string body = "[timeout:10][out:json];(node(around:"+E+","+lat+","+lon+");way(around:"+E+","+lat+","+lon+"););relation(around:"+E+","+lat+","+lon+");";
            string body = "[timeout:10][out:json];(node(around:"+E+","+lat+","+lon+");way(around:"+E+","+lat+","+lon+"););out tags geom("+bbox+");relation(around:"+E+","+lat+","+lon+");out geom("+bbox+");";
            cout<<body<<endl;
            
            auto response = cpr::Get(cpr::Url{"https://www.overpass-api.de/api/interpreter"},
            cpr::Body(body));
            auto json = json::parse(response.text);
            std::cout << json.dump(4) << std::endl;
        }
        
        for(POI* p : data[index]->pois)
        {
            
        }
    }
    threadRunning=0;
}


vector <LogData*> LogViewer::getPastPositions()
{
    double index = (current_time/steptime) - data[0]->time/steptime;
    double fac = (double)resolution/1000000;
    vector <LogData*> ret;
    for(double i = 0.; i<=index;i+=fac*timescale)
    {
        ret.push_back(data[(int)i]);
    }
    return ret;
}