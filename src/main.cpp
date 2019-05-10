#include <iostream>
#include <cpr/cpr.h>
#include <json.hpp>
#include <thread>
#include <POI.h>
#include <unistd.h>
#include <Definitions.h>
using namespace std;

void run()
{
    time_t started = time(0);
    while(!endd)
    {
        while(paused)
            usleep(250000);
        if(paused)
            continue;
        if(!rev)
            current_time+=.25 * timescale;
        else
            current_time-=.25 * timescale;
        // started = time(0);
        usleep(250000);//microseconds
        cout<<"Current time is: "<<current_time<<" P: "<<paused<<" R: "<<rev<<endl;
    }
}

int main(int argc, char* argv[])
{
    auto response = cpr::Get(cpr::Url{"https://www.overpass-api.de/api/interpreter"},
    cpr::Body("[timeout:10][out:json];(node(around:22.5,37.77944,-122.42526);way(around:22.5,37.77944,-122.42526););out tags geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);relation(around:22.5,37.77944,-122.42526);out geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);"));
    auto json = nlohmann::json::parse(response.text);
    std::cout << json.dump(4) << std::endl;
    // Runner r;
    thread th(run);
    cout<<"1: "<<current_time<<endl;
    // th.join();
    char c = '\0';
    while(c!='E')
    {
        cin>>c;
        if(c=='E')
        {
            endd = 1;
            th.join();
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
}