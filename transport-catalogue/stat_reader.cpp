#include "stat_reader.h"

#include <iomanip>

using namespace std;

void print::BusInformation(const information::Bus& route)
{
	cout << "Bus " << route.GetNumberBus() << ": " << route.GetStopsOnRoute() << " stops on route, " << route.GetUniqueStops() << " unique stops, " << setprecision(6) << route.GetSumFactDistance() << " route length, " << route.GetCurvature() << " curvature" << endl;
}

void print::NotFoundBus(const string& a)
{
	cout << "Bus "s << a << ": not found" << endl;
}

void print::NotFoundStops(const string& a)
{
	cout << "Stop " << a << ": not found" << endl;
}

void print::FoundInBusOnRoute(const information::Stop& stop)
{
	if (stop.GetBusRoute().empty())
	{
		cout << "Stop " << stop.GetName() << ": no buses" << endl;
	}
	else
	{
		cout << "Stop " << stop.GetName() << ": buses";
		for (const auto& bus : stop.GetBusRoute())
		{
			cout << " " << bus;
		}
		cout << endl;
	}
}

