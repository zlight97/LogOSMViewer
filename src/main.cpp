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
    for(int i =0; i<threads.size(); i++)
    {
        threads[i]->join();
        delete threads[i];
        delete viewers[i];
    }
    th.join();
}