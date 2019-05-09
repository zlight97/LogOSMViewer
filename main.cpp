#include <iostream>
#include <cpr/cpr.h>
#include "json/single_include/nlohmann/json.hpp"
#include "POI.h"
using namespace std;

int main(int argc, char* argv[])
{
    auto response = cpr::Get(cpr::Url{"https://www.overpass-api.de/api/interpreter"},
    cpr::Body("[timeout:10][out:json];(node(around:22.5,37.77944,-122.42526);way(around:22.5,37.77944,-122.42526););out tags geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);relation(around:22.5,37.77944,-122.42526);out geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);"));
    auto json = nlohmann::json::parse(response.text);
    std::cout << json.dump(4) << std::endl;
}