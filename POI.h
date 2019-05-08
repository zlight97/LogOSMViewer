#ifndef POI_H
#define POI_H

#include <vector>
#include <unordered_map>
using namespace std;

enum poi_type
{
    RELATION,
    NODE,
    WAY,
    NONE
};

class POI
{
    unordered_map <string,string>tags;
    poi_type type;
public:
    POI(poi_type type);
    ~POI();
    poi_type getPOIType() {return type;}
    void addTag(string name, string desc);
    

};

#endif