#include <iostream>
#include <cpr/cpr.h>
#include <json.hpp>
#include <thread>
#include <POI.h>
#include <unistd.h>
#include <Definitions.h>
#include <LogViewer.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

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
    vector <string> paths;

    for (const auto & entry : fs::directory_iterator("../Logs"))
        paths.push_back(entry.path());

    vector <thread*> threads;
    vector <LogViewer*> viewers;
    thread th(run);
    int i = 0;
    for(string s : paths)
    {
        LogViewer *l = new LogViewer(s, i);
        thread *logg = new thread(&LogViewer::run, l); 
        i++;
    }
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
    for(thread *t : threads)
        t->join();
    th.join();
}