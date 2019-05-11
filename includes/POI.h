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

class POI
{
    vector <Tag*>tags;
    poi_type type;
public:
    POI(poi_type type);
    ~POI();
    poi_type getPOIType() {return type;}
    void addTag(Tag *t);
    void addTag(string name, string desc);
    

};

#endif