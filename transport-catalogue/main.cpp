#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main()
{
    vector<reader::Stop> inPutStops;
    vector<reader::Bus> inPutBus;
    reader::ReadingSourceData(inPutStops, inPutBus);

    map<string, information::Stop> existingStops;
    map<pair<string, string>, size_t> distanceBetweenStops;
    for (auto& elem : inPutStops)
    {
        existingStops.insert(make_pair(elem._stop, information::Stop(elem._lat, elem._lng, elem._stop)));

        for (const auto& [stopName, stopDistance] : elem._distanceToStops)
        {
            distanceBetweenStops[{elem._stop, stopName}] = stopDistance;

        }
    }

    map<string, information::Bus> existingBus;
    for (auto& elem : inPutBus)
    {
        existingBus.insert(make_pair(elem._numberBus, information::Bus::CreateBusRoute(elem._numberBus, elem._stopBus, elem._isCircleOrNot, existingStops,distanceBetweenStops)));
    }

    vector<reader::QueryBusStops> inPutFindBusStops;
    reader::ReadingBusesAndStopsLookingFor(inPutFindBusStops);
    for (auto& elem : inPutFindBusStops)
    {
        if (elem._isBus)
        {
            auto it = existingBus.find(elem._name);

            if (it != existingBus.end())
            {
                print::BusInformation(it->second);
            }
            else
            {
                print::NotFoundBus(elem._name);
            }
        }
        else
        {
            auto it = existingStops.find(elem._name);

            if (it == existingStops.end())
            {
                print::NotFoundStops(elem._name);
            }
            else
            {
                print::FoundInBusOnRoute(it->second);
            }
        }
    }

    return 0;
}