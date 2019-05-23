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
LogViewer::LogViewer(string file, int ID, LogData* massD)
{
    massData = massD;
    threadRunning = 0;
    id = ID;
    fstream f;
    f.open(file, ios::in);
    string line, word, temp;
    vector<string> v;
    //this while parses a log file, splitting it into each section, and adding it to a LogData vector called data
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

    //each file had a different ammount of time between each step, but it remained consistant between each file
    //This is used to find our index based on the real-time
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
{//cleaning pointers
    for(int i = 0; i <data.size(); i++)
    {
        for(int j = 0; j<data[i]->pois.size();j++)
        {
            delete data[i]->pois[j];
        }
        data[i]->pois.clear();
        delete data[i];
    }
    data.clear();
}

void LogViewer::run()
{   
    //the function for each threaded LogViewer. This function handles when it print's it's data based on the current time
    //It sleeps at the same rate as the master clock so each thread will poll once per clock tick
    while(1)
    {
        while(paused)
        {
            usleep(resolution);
        }
        double index = (current_time/steptime) - data[0]->time/steptime;
        int i = (int) index;
        if(i>=0&&i<data.size())//if we arent in valid range there is no playback
        {
            cout<<"ID: "<<id<<" TIME: "<<data[i]->time<<"\nnLat: "<<data[i]->nLat<<" nLong: "<<data[i]->nLong<<" nElev: "<<data[i]->nE<<endl;
            cout<<"gpsLat: "<<data[i]->gpsLat<<" gpsLong: "<<data[i]->gpsLong<<" gpsElev: "<<data[i]->gpsE<<endl;
        
            double l = data[i]->nLat, lo = data[i]->nLong;
            int n=0;
            // for(POI* p : massData->pois)
            for(int i = 0; i<massData->pois.size(); i++)
            {
                if(massData->pois[i]->getPOIType()!=WAY)
                    continue;
                Bound b = massData->pois[i]->getBounds();
                // cout<<b.minLat<<b.maxLat<<b.minLon<<b.maxLon<<endl<<l<<lo<<endl;
                if(massData->pois[i]->building()&&l>b.minLat&&l<b.maxLat&&lo>b.minLon&&lo<b.maxLon)
                {
                    bool flag = 0;
                    for(inside in : isInside)
                    {
                        if(i==in.number)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 1)
                        continue;
                    else
                    {
                        inside in;
                        in.number=i;
                        in.startTime = current_time;
                        isInside.push_back(in);
                    }
                }
            }
            for(int i = 0; i<isInside.size();i++)
            {
                Bound b = massData->pois[isInside[i].number]->getBounds();
                if(!(l>b.minLat&&l<b.maxLat&&lo>b.minLon&&lo<b.maxLon))
                {
                    cout<<"Node: "<<id<<" has left building after: "<<current_time-isInside[i].startTime<<"seconds.\nBuilding Data:\n";
                    massData->pois[isInside[i].number]->printQueriedInfo();
                    isInside.erase(isInside.begin()+i);
                    i--;
                }
            }
        }
        if(endd)
            return;
        usleep(resolution);
    }
}

void LogViewer::query()
{
    //the secondary thread of each LogViewer, which gathers queried data without disrupting normal playback
    //this can be called as many times as wanted during a data-set and all data will be stored for potential later use
    threadRunning=1;
    double index = (current_time/steptime) - data[0]->time/steptime;//calculating index based off time
    int i = (double) index;
    if(i>=0&&i<data.size())//if we are not in valid range we do not check
    {
        if(data[i]->pois.size()==0)//if there isn't already data at this point
        {
            //this block determines the data that is going to be sent to the overpass API
            string lat = to_string(data[i]->nLat);
            string lon = to_string(data[i]->nLong);
            string E = "500";//to_string(data[i]->nE);
            double latFac = 0.00042076252;//these factors were determined by playing around with OSM and viewing how it makes it's calls to overpass
            double lonFac = 0.00205188989;
            string bbox = "37.7330,-122.5128,37.8096,-122.3453";//to_string(data[i]->nLat-latFac)+","+to_string(data[i]->nLong-lonFac)+","+to_string(data[i]->nLat+latFac)+","+to_string(data[i]->nLong+lonFac);
            string body = "[timeout:10][out:json];(node(around:"+E+","+lat+","+lon+");way(around:"+E+","+lat+","+lon+"););out tags geom("+bbox+");";//relation(around:"+E+","+lat+","+lon+");out geom("+bbox+");";
            //"[timeout:10][out:json];(node(around:500,37.7718,-122.4347);way(around:500,37.7718,-122.4347););out tags geom(37.7330,-122.5128,37.8096,-122.3453);";
                                                                                                                        //commenting after this ;^ and adding "; will remove relations, which in turn removes a lot of garbage data
            //I chose to use overpass as it does all of the searching for me, and gives me nearby results, which are easier to parse
            auto response = cpr::Get(cpr::Url{"https://www.overpass-api.de/api/interpreter"},
            cpr::Body(body));
            try{//try catch is to catch timeouts or other null-data. Though this should never happen, it is possible with a bad connection the network won't usually error, but the json parsing will if this occurs
                auto json = json::parse(response.text);
                for(int jI = 0; jI<jEle.size(); jI++)
                {
                    POI *p;
                    //all json objects in this api will have a type field
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
                    //these hold potential useful data, which are described as is
                    if(!jEle[jI]["lat"].is_null() && !jEle[jI]["lon"].is_null())//lat lon are only for nodes
                    {
                        p->setCoord(jEle[jI]["lat"], jEle[jI]["lon"]);
                    }
                    if(!jEle[jI]["tags"].is_null())
                    {
                        //use an iterator as that allows me to find the field name as well as value of tags
                        //this allows for me to accept tags that I haven't seen before
                        //this is good because there are way too many tags to go through on OSM
                        for (auto it = jEle[jI]["tags"].begin(); it != jEle[jI]["tags"].end(); ++it)
                        {
                            string field = it.key();
                            //uncommenting this field will remove address data from the relevant tag list.
                            //if discovered, other useless tags can be added in a similar manner
                            // if(field.find("addr")!=-1)
                            //     continue;
                            p->addTag(field, it.value());
                        }
                    }
                    //these are the outlines of ways (and maybe sometimes relations?)
                    if(!jEle[jI]["geometry"].is_null())
                    {
                        for(int n = 0; n<jEle[jI]["geometry"].size();n++)
                        {
                            if(jEle[jI]["geometry"][n].is_null())
                                p->addGeom();//for some reason there are usually many null points, so we duplicate this in our data structure
                            else
                                p->addGeom(jEle[jI]["geometry"][n]["lat"],jEle[jI]["geometry"][n]["lon"]);
                        }

                    }
                    //bounds are only for non-nodes
                    if(!jEle[jI]["bounds"].is_null())
                    {
                        p->setBounds(jEle[jI]["bounds"]["minlat"], jEle[jI]["bounds"]["minlon"], jEle[jI]["bounds"]["maxlat"], jEle[jI]["bounds"]["maxlon"]);
                    }
                }
            }catch(nlohmann::detail::parse_error e)
            {
                cerr<<"Could not parse data\n";
            }
        }

        //once we have processed data, or determined that it was processed prior, we print the data
        cout<<"Points of interest at timestep: "<<data[index]->time<<endl;
        for(POI* p : data[index]->pois)
        {
            p->printQueriedInfo();
        }
            
    }
    //this atomic boolean lets me safely delete threads that have finished their query
    threadRunning=0;
}

//this creates a vector of past positions based on the current timestep. 
//This is designed to allow paths to be drawn in p2
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

//makes creating a thread query externally a little simpler
thread* LogViewer::createThreadedQuery()
{
    return new thread(&LogViewer::query, this);
}

//used for printing each timestep's queried data
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

void LogViewer::writeQueriedInfo(string filename)
{
    filename = "../geoJSON/"+filename;
    ifstream infile(filename);
    json j;
    infile>>j;
    infile.close();
    // for(int i = 0; i<data.size();i++)
    // {
        if(massData!=NULL&&massData->pois.size()>0)
        {
            for(POI* p : massData->pois)
            {
                if(p==NULL||p->getPOIType()==RELATION)
                    continue;
                json feature;
                feature["type"]="Feature";
                if(p->getPOIType()==NODE)
                {
                    if(p->getTags().size()==0)
                        continue;
                    Coord g = p->getCoord();
                    feature["geometry"]["type"]="Point";
                    feature["geometry"]["coordinates"]={g.lon, g.lat};
                }else if(p->getPOIType()==WAY)
                {
                    int flag = 0;
                    for(Tag* t : p->getTags())
                    {
                        if(t->name=="building")
                        {
                            flag = 1;
                            if(t->desc!="yes")
                                continue;
                        }
                        if(t->name=="lanes")
                            flag = 2;
                        
                    }if(flag==0)
                        continue;
                    else if(flag==1){
                        feature["geometry"]["type"]="Polygon"; 
                        feature["geometry"]["coordinates"]={};
                        for(Coord *c : p->getGeom())
                        {
                            if(c!=NULL)
                                feature["geometry"]["coordinates"][0].push_back({c->lon,c->lat});
                        }
                        if(feature["geometry"]["coordinates"][0].size()<4)
                            continue;
                        if(feature["geometry"]["coordinates"][0].size()>0)
                            feature["geometry"]["coordinates"][0].push_back(feature["geometry"]["coordinates"][0][0]);
                    }
                    else if(flag == 2)
                    {
                        feature["geometry"]["type"]="LineString"; 
                        feature["geometry"]["coordinates"]={};
                        for(Coord *c : p->getGeom())
                        {
                            if(c!=NULL)
                                feature["geometry"]["coordinates"].push_back({c->lon,c->lat});
                        }
                        if(feature["geometry"]["coordinates"].size()<2)
                            continue;
                    }

                }else if(p->getPOIType()==RELATION)
                {

                }
                else
                {

                }
                feature["properties"] = {};
                for(Tag* t : p->getTags())
                {
                    if(feature["properties"].is_null())
                        feature["properties"][t->name]=t->desc;
                    else
                        feature["properties"].push_back({t->name,t->desc});
                }
                // cout<<"???????\n"<<feature.dump(4)<<endl;
                if(j["features"].is_null())
                    j["features"] = {feature};
                else
                    j["features"].push_back(feature);
                // cout<<j.dump(4)<<endl;
            }
        }
    // }
    // cout<<"!!!!!!!!!!!!!!\n"<<j.dump(4)<<endl;
    ofstream output(filename);
    output<<j.dump(4)<<endl;
    output.close();
    writePastLocations(filename);
}

void LogViewer::writePastLocations(string filename)
{
    // filename = "../geoJSON/"+filename;
    ifstream infile(filename);
    json j;
    infile>>j;
    infile.close();
    vector <LogData*> v = getPastPositions();
    if(v.size()<=1)
        return;
    json feature;
    feature["type"]="Feature";
    feature["geometry"]["type"]="LineString"; 
    feature["geometry"]["coordinates"]={};
    for(int i = 0; i<v.size();i+=50)
    {
        if(v[i]!=NULL)
            feature["geometry"]["coordinates"].push_back({v[i]->nLong,v[i]->nLat, v[i]->nE});
    }
    long co = ((color.R & 0xff) << 16) + ((color.G & 0xff) << 8) + (color.B & 0xff);
    feature["properties"]["stroke"] = "#"+to_string(co);
    feature["properties"]["LogPATH"]=to_string(id);
    j["features"].push_back(feature);
    ofstream output(filename);
    output<<j.dump(4)<<endl;
    output.close();
}