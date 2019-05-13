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

/*
*This file holds the main function, which handles starting the entire project
*this includes starting a task which handles the run() function, which acts as the counter for the global clock
*
*Main also handles taking user input
*The commands for this are:
*E - exit
*R - toggle reverse
*T d - Set timescale to double d
*Q n - query LogViewerID n at current position (negative n will query all IDS)
    *this forks a new thread  to handle the API calls, query will usually complete in about a second/query based on connection speed and query size
*P - toggle pause/play
*A - show data from all queried spots accross all LogViewers
*/

//initilization of extern variables from Definitions.h
double current_time = 0, timescale = 1.;
bool paused =0, endd = 0, rev = 0;
//resolution handles the polling rate for the clock
//Faster rate (smaller number) means more polls
int resolution = resolution_start;

//Handles the clock based on all possible variables
void run()
{
    sleep(1);//this lets the threads initilize so they can playback from 0 if available
    double fac = (double)resolution/1000000;
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
        usleep(resolution);//microseconds
        cout<<"MAIN Current time is: "<<current_time<<" P: "<<paused<<" R: "<<rev<<" T: "<<timescale<<endl;
    }
}

int main(int argc, char* argv[])
{
    
    vector <string> paths;

    //this finds all logs in the Logs folder so we can turn them into LogViewers
    for (const auto & entry : fs::directory_iterator("../Logs"))
        paths.push_back(entry.path());

    vector <thread*> threads;
    vector <thread*> netThreads;
    vector <LogViewer*> viewers;
    vector <int> netThreadIds;
    //this thread runs the clock Run() function
    thread th(run);
    int id = 0;

    //initilizing all the log files as their own thread
    for(string s : paths)
    {
        LogViewer *l = new LogViewer(s, id);
        thread *logg = new thread(&LogViewer::run, l); 
        threads.push_back(logg);
        viewers.push_back(l);
        id++;
    }
    char c = '\0';
    while(c!='E')
    {
        cin>>c;
        if(c=='E')
        {
            paused = 0;
            endd = 1;
            break;
        }
        if(c=='R')
            rev = !rev;
        if(c=='T')
        {
            double i = 1;
            cin>>i;
            if(i<=0.||i>=10.)
                continue;
            timescale = i;
            resolution = resolution_start/timescale;
        }
        if(c=='P')
            paused = !paused;
        if(c=='Q')
        {
            int i;
            cin>>i;
            if(i<0)
            {
                for(LogViewer* l : viewers)
                {
                    // if(l->isNetThreadRunning())
                    //     continue;
                    netThreads.push_back(l->createThreadedQuery());
                    netThreadIds.push_back(l->getID());
                }
            }
            else if(i<viewers.size()&&!viewers[i]->isNetThreadRunning())
            {
                netThreads.push_back(viewers[i]->createThreadedQuery());
                netThreadIds.push_back(i);
            }
            
        }
        if(c=='A')
        {
            int temp = 0;
            for(LogViewer *v : viewers)
            {
                cout<<"Log ID: "<<temp++;
                v->printAllQueriedInfo();
            }
        }

        //this works to free some finished threads. Only runs on each input - this doesn't matter as querying a new task requires input
        for(int i = 0; i<netThreads.size(); i++)
        {
            if(viewers[netThreadIds[i]]->isNetThreadRunning())
            {
                netThreads[i]->join();
                delete netThreads[i];
                netThreadIds.erase(netThreadIds.begin()+i);
                netThreads.erase(netThreads.begin()+i);
            }
        }
    }
    //clean up all of the threads before we exit
    for(int i =0; i<threads.size(); i++)
    {
        threads[i]->join();
        delete threads[i];
        delete viewers[i];
    }
    for(int i = 0; i<netThreads.size(); i++)
    {
        netThreads[i]->join();
        delete netThreads[i];
    }
    th.join();
}