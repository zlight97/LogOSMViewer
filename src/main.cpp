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
bool paused =0, endd = 0, rev = 0, quer = 0;
int resolution = resolution_start, queryID = -1;

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
    vector <thread*> netThreads;
    vector <LogViewer*> viewers;
    vector <int> netThreadIds;
    thread th(run);
    int id = 0;
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
                    // thread *net = new thread(&LogViewer::query, l);
                    netThreads.push_back(l->createThreadedQuery());
                    netThreadIds.push_back(l->getID());
                }
            }
            else if(i<viewers.size()&&!viewers[i]->isNetThreadRunning())
            {
                // thread *net = new thread(&LogViewer::query, viewers[i]);
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