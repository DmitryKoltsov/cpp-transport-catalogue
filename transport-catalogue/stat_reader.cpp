#include "stat_reader.h"

#include <iomanip>

using namespace std;

void print::BusInformation(std::ostream& stream, const information::Bus& route)
{
	stream << "Bus " << route.GetNumberBus() << ": " << route.GetStopsOnRoute() << " stops on route, " << route.GetUniqueStops() << " unique stops, " << setprecision(6) << route.GetSumFactDistance() << " route length, " << route.GetCurvature() << " curvature" << endl;
}

void print::NotFoundBus(std::ostream& stream, const string& a)
{
	stream << "Bus "s << a << ": not found" << endl;
}

void print::NotFoundStops(std::ostream& stream, const string& a)
{
	stream << "Stop " << a << ": not found" << endl;
}

void print::FoundInBusOnRoute(std::ostream& stream, const information::Stop& stop)
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

