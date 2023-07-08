#include "geo.h" 
#include "transport_catalogue.h"
using namespace std; 
 
namespace information 
{ 

	void Catalogue::AddStops(const string& stop, double lat, double lng, const map<string, int>& distanceToStops) 
	{ 
		existingStops.insert(make_pair(stop, domain::Stop(lat, lng, stop)));
 
		for (const auto& [stopName, stopDistance] : distanceToStops) 
		{ 
			distanceBetweenStops[{stop, stopName}] = stopDistance; 
		} 
	} 
 
	void Catalogue::AddBus(const string& numberBus, vector<string>& stopBus, bool isCircleOrNot) 
	{ 
			existingBus.insert(make_pair(numberBus, domain::Bus::CreateBusRoute(numberBus, stopBus, isCircleOrNot, existingStops, distanceBetweenStops)));
	} 
 
	std::optional<domain::Bus> Catalogue::FindBus(const string& name) const
	{ 
		auto it = existingBus.find(name); 
 
		if (it != existingBus.end()) 
		{ 
			return it->second; 
		} 
		return std::nullopt; 
	} 
 
	std::optional<domain::Stop> Catalogue::FindStops(const string& name) const
	{ 
		auto it = existingStops.find(name); 
 
		if (it == existingStops.end()) 
		{ 
			return std::nullopt; 
		} 
		return it->second; 
	}
	const std::map<std::string, domain::Bus>& Catalogue::GetExistingBus() const
	{
		return existingBus;
	}

} 