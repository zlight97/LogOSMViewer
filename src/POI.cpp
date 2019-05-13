#include <POI.h>
/*
 *POI was what I decided to call my class for storing OSM data
 *These points of interest hold ways nodes and relations and their data
 */
POI::POI(poi_type type)
{
    this->type = type;
}
POI::~POI()
{
    //making sure delete pointers to avoid leaks
    for(int i = 0; i<tags.size();i++)
        delete tags[i];
    tags.clear();
    for(int i =0; i<geometry.size();i++)
        delete geometry[i];
    geometry.clear();
}
void POI::addTag(Tag *t)
{
    tags.push_back(t);
}
void POI::addTag(string name, string desc)
{
    Tag *t = new Tag;
    t->name = name;
    t->desc = desc;
    tags.push_back(t);
}
void POI::addGeom(double lat, double lon)
{
    Coord *c = new Coord;
    c->lat = lat;
    c->lon = lon;
    geometry.push_back(c);
}
void POI::addGeom()
{
    geometry.push_back(NULL);
}
void POI::setBounds(double minLat, double minLon, double maxLat, double maxLon)
{
    bounds.minLat = minLat;
    bounds.minLon = minLon;
    bounds.maxLat = maxLat;
    bounds.maxLon = maxLon;
}

void POI::setCoord(double lat, double lon)
{
    nodeCoord.lat = lat;
    nodeCoord.lon = lon;
}

//this is just used for printing easier
string POI::getTypeStr()
{
    switch(type)
    {
        case RELATION:
        return "relation";
        case NODE:
        return "node";
        case WAY:
        return "way";
        default:
        return "none";
    }
}

//print relevant data. As i do not know how much data we will want, I am printing all data for now
void POI::printQueriedInfo()
{
    cout<<"Type: "<<getTypeStr()<<
    "\nTags:\n{";
    if(getTags().size()==0)
        cout<<"\n\tNo Tags\n}\n";
    else{
        for(Tag* t:getTags())
        {
            cout<<"\t"<<t->name<<"\t:\t\""<<t->desc<<"\"\n";
        }
        cout<<"}\n";
    }
    if(getPOIType() == NODE)
    {
        Coord c = getCoord();
        cout<<"Latitude: "<<c.lat<<" Longitude: "<<c.lon<<endl;
    }
    else
    {
        cout<<"Bounds:\n{\n\tminlat: "<<bounds.minLat<<"\n\tmaxlat: "<<bounds.maxLat<<"\n\tminlon: "<<bounds.minLon<<"\n\tmaxlon: "<<bounds.maxLon<<"\n}\n";
    }
    cout<<endl;
    
}