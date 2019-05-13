#ifndef POI_H
#define POI_H

#include <vector>
#include <string>
#include <Definitions.h>
#include <iostream>
using namespace std;

enum poi_type
{
    RELATION,
    NODE,
    WAY,
    NONE
};

struct Tag
{
    string name;
    string desc;
};

struct Coord
{
    double lat;
    double lon;
};

struct Bound
{
    double minLat;
    double maxLat;
    double minLon;
    double maxLon;
};

class POI
{
    vector <Tag*>tags;
    poi_type type;
    Coord coordinates;
    vector <Coord*> geometry;
    Bound bounds;
    Coord nodeCoord;
public:
    POI(poi_type type);
    ~POI();
    poi_type getPOIType() {return type;}
    vector <Tag*> getTags() {return tags;}
    vector<Coord*> getGeom() {return geometry;}
    Bound getBounds() {return bounds;}
    void addTag(Tag *t);
    void addTag(string name, string desc);
    void addGeom(double lat, double lon);
    void addGeom();//adding null
    void setBounds(double minLat, double minLon, double maxLat, double maxLon);
    void setCoord(double lat, double lon);
    Coord getCoord() { return nodeCoord;}
    

};

#endif