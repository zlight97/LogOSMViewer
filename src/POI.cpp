#include <POI.h>
POI::POI(poi_type type)
{
    this->type = type;
}
POI::~POI()
{
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