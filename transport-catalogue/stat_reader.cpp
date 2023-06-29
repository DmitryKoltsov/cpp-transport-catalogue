#include "stat_reader.h"

#include <iomanip>
#include "input_reader.h"

using namespace std;

namespace print
{

    void ReadAndExecuteQueries(const information::Catalogue& catalogue)
    {
        vector<reader::QueryBusStops> inPutFindBusStops;
        reader::ReadQueries(inPutFindBusStops);
        for (auto& elem : inPutFindBusStops)
        {
            if (elem._isBus)
            {
                auto findingBus = catalogue.FindBus(elem._name);
                if (findingBus.has_value())
                {
                    BusInformation(std::cout, *findingBus);
                }
                else
                {
                    NotFoundBus(std::cout, elem._name);
                }
            }
            else
            {
                auto findingStop = catalogue.FindStops(elem._name);
                if (findingStop.has_value())
                {

                    FoundInBusOnRoute(std::cout, *findingStop);
                }
                else
                {
                    NotFoundStops(std::cout, elem._name);
                }
            }
        }
    }

    void BusInformation(std::ostream& stream, const information::Bus& route)
    {
        stream << "Bus " << route.GetNumberBus() << ": " << route.GetStopsOnRoute() << " stops on route, " << route.GetUniqueStops() << " unique stops, " << setprecision(6) << route.GetSumFactDistance() << " route length, " << route.GetCurvature() << " curvature" << endl;
    }

    void NotFoundBus(std::ostream& stream, const string& a)
    {
        stream << "Bus "s << a << ": not found" << endl;
    }

    void NotFoundStops(std::ostream& stream, const string& a)
    {
        stream << "Stop " << a << ": not found" << endl;
    }

    void FoundInBusOnRoute(std::ostream& stream, const information::Stop& stop)
    {
        if (stop.GetBusRoute().empty())
        {
            stream << "Stop " << stop.GetName() << ": no buses" << endl;
        }
        else
        {
            stream << "Stop " << stop.GetName() << ": buses";
            for (const auto& bus : stop.GetBusRoute())
            {
                stream << " " << bus;
            }
            stream << endl;
        }
    }
}

