#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

information::Catalogue ReadData()
{
    vector<reader::Stop> inPutStops;
    vector<reader::Bus> inPutBus;
    reader::ReadData(inPutStops, inPutBus);

    information::Catalogue catalogue;

    for (auto& elem : inPutStops)
    {
        catalogue.AddStops(elem._stop, elem._lat, elem._lng, elem._distanceToStops);
    }
    for (auto& elem : inPutBus)
    {
        catalogue.AddBus(elem._numberBus, elem._stopBus, elem._isCircleOrNot);
    }
    return catalogue;
}

void ExistingData(information::Catalogue catalogue)
{
    vector<reader::QueryBusStops> inPutFindBusStops;
    reader::ReadQueries(inPutFindBusStops);
    for (auto& elem : inPutFindBusStops)
    {
        if (elem._isBus)
        {
            auto findingBus = catalogue.FindingBus(elem._name);
            if (findingBus.has_value())
            {
                print::BusInformation(std::cout, *findingBus);
            }
            else
            {
                print::NotFoundBus(std::cout, elem._name);
            }
        }
        else
        {
            auto findingStop = catalogue.FindingStops(elem._name);
            if (findingStop.has_value())
            {

                print::FoundInBusOnRoute(std::cout, *findingStop);
            }
            else
            {
                print::NotFoundStops(std::cout, elem._name);
            }
        }
    }
}

int main()
{
    auto catalogue = ReadData();
    ExistingData(catalogue);

    return 0;
}