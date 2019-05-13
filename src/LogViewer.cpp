#include <LogViewer.h>
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>
#include <json.hpp>
#include <unistd.h>
#include <random>
#define jEle json["elements"]
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

    //this creates random colors based off the node's id
    //while it is possible for two nodes to share the same color, it is extremely unlikely
    time_t now = time(0);
    now = now/10 * ID;
    srand(now);
    srand48(now);
    color.R = rand()%255;
    color.G = rand()%255;
    color.B = rand()%255;
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
            string body = "[timeout:10][out:json];(node(around:"+E+","+lat+","+lon+");way(around:"+E+","+lat+","+lon+"););out tags geom("+bbox+");relation(around:"+E+","+lat+","+lon+");out geom("+bbox+");";
            
            auto response = cpr::Get(cpr::Url{"https://www.overpass-api.de/api/interpreter"},
            cpr::Body(body));
            try{
                auto json = json::parse(response.text);
            for(int jI = 0; jI<jEle.size(); jI++)
            {
                POI *p;
                if(jEle[jI]["type"]=="node")
                {
                    p = new POI(NODE);
                    data[index]->pois.push_back(p);
                }
                else if(jEle[jI]["type"]=="way")
                {
                    p = new POI(WAY);
                    data[index]->pois.push_back(p);
                }
                else if(jEle[jI]["type"]=="relation")
                {
                    p = new POI(RELATION);
                    data[index]->pois.push_back(p);
                }
                else
                {
                    cout<<"UNTYPED NODE???"<<endl;//this should never happen
                    continue;
                }
                if(!jEle[jI]["lat"].is_null() && !jEle[jI]["lon"].is_null())
                {
                    p->setCoord(jEle[jI]["lat"], jEle[jI]["lon"]);
                }
                if(!jEle[jI]["tags"].is_null())
                {
                    for (auto it = jEle[jI]["tags"].begin(); it != jEle[jI]["tags"].end(); ++it)
                    {
                        string field = it.key();
                        if(field.find("addr")!=-1)
                            continue;
                        p->addTag(field, it.value());
                    }
                }
                if(!jEle[jI]["geometry"].is_null())
                {
                    for(int n = 0; n<jEle[jI]["geometry"].size();n++)
                    {
                        if(jEle[jI]["geometry"][n].is_null())
                            p->addGeom();
                        else
                            p->addGeom(jEle[jI]["geometry"][n]["lat"],jEle[jI]["geometry"][n]["lon"]);
                    }

                }
            }
            }catch(nlohmann::detail::parse_error e)
            {
                cerr<<"Could not parse data:\n"<<e.what();
            }
        }
        cout<<"Points of interest at timestep: "<<data[index]->time<<endl;
        for(POI* p : data[index]->pois)
        {
            p->printQueriedInfo();
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

thread* LogViewer::createThreadedQuery()
{
    return new thread(&LogViewer::query, this);
}

void LogViewer::printAllQueriedInfo()
{
    for(int i = 0; i<data.size();i++)
    {
        if(data[i]->pois.size()>0)
        {
            cout<<"Points of interest at timestep: "<<data[i]->time<<endl;
            for(POI* p : data[i]->pois)
            {
                p->printQueriedInfo();
            }
        }
    }
}