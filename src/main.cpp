#include <iostream>
#include <cpr/cpr.h>
#include <json.hpp>
#include <thread>
#include <POI.h>
#include <unistd.h>
#include <Definitions.h>
#include <LogViewer.h>
using namespace std;
double current_time = 0, timescale = 1.;
bool paused =0, endd = 0, rev = 0;
int resolution = 250000;
void run()
{
    double fac = (double)resolution/1000000;
    time_t started = time(0);
    while(!endd)
    {
        while(paused)
            usleep(resolution);
        if(paused)
            continue;
        if(!rev)
            current_time+=fac * timescale;
        else
            current_time-=fac * timescale;
        // started = time(0);
        usleep(resolution);//microseconds
        cout<<"MAIN Current time is: "<<current_time<<" P: "<<paused<<" R: "<<rev<<endl;
    }
}

int main(int argc, char* argv[])
{
    // auto response = cpr::Get(cpr::Url{"https://www.overpass-api.de/api/interpreter"},
    // cpr::Body("[timeout:10][out:json];(node(around:22.5,37.77944,-122.42526);way(around:22.5,37.77944,-122.42526););out tags geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);relation(around:22.5,37.77944,-122.42526);out geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);"));
    // auto json = nlohmann::json::parse(response.text);
    // std::cout << json.dump(4) << std::endl;
    thread th(run);
    LogViewer l("ty5OWH3TUHlxTbwZXr3KHBzDTZZpeMVFaTjGrMqNKhpNbsfAKvF0G6pRIap0dkdt-2019-04-16_13-05-04.nav-project.csv");
    thread v(&LogViewer::run, &l);
    LogViewer l2("4e7485cfe0c552a50112f33c573dca8c4e174786a59a6e407a589aa6d1d71d7a-2019-04-16_13-08-45.nav-project.csv");
    thread v2(&LogViewer::run, &l2);
    char c = '\0';
    while(c!='E')
    {
        cin>>c;
        if(c=='E')
        {
            endd = 1;
            break;
        }
        if(c=='R')
            rev = !rev;
        if(c=='T')
        {
            double i = 1;
            cin>>i;
            cout<<i<<endl;
            timescale = i;
        }
        if(c=='P')
            paused = !paused;
    }
    v.join();
    v2.join();
    th.join();
}