#include "domain.h" 
#include "geo.h"  
using namespace std; 
 
domain::Stop::Stop(double& lat, double& lng, const string& stop) : _stop(stop) 
{ 
	_coords.lat = lat; 
	_coords.lng = lng; 
} 
 
 
bool domain::Stop::operator<(const Stop& other) const 
{ 
	return _stop < other._stop; 
} 
 
double domain::Stop::CalculateDistanceToStop(const Stop& other) const 
{ 
	return geo::ComputeDistance(_coords, other._coords); 
} 
 
const string& domain::Stop::GetName() const 
{ 
	return _stop; 
} 
 
const std::set<std::string>& domain::Stop::GetBusRoute() const 
{ 
	return _busRoutes; 
} 
 
void domain::Stop::AddRoute(const std::string& numberBus) 
{ 
	_busRoutes.insert(numberBus); 
} 
geo::Coordinates domain::Stop::GetCoords() const 
{ 
	return _coords; 
} 
 
 
domain::Bus::Bus(const string& numberBus, const vector<string>& stopBus, size_t stopsOnRoute, size_t uniqueStops, size_t sumFactDist, double curvate, bool isCircle) : 
	_numberBus(numberBus), 
	_stopBus(stopBus), 
	_stopsOnRoute(stopsOnRoute), 
	_uniqueStops(uniqueStops), 
	_sumFactDist(sumFactDist), 
	_curvate(curvate), 
	_isCircle(isCircle) 
{ 
 
} 
 
bool domain::Bus::operator<(const Bus& other) const 
{ 
	return _numberBus < other._numberBus; 
} 
 
size_t domain::Bus::CurvateLenght() 
{ 
	return size_t(); 
} 
 
domain::Bus domain::Bus::CreateBusRoute(const string& numberBus, vector<string>& stopBus, bool isCircleOrNot, map<string, Stop>& existingStops, map<pair<string, string>, size_t>& distanceBetweenStops) 
{ 
	size_t stopsOnRoute = stopBus.size(); 
	size_t uniqueStops = 0; 
	double sumDistance = 0.0; 
	size_t distance = 0; 
	size_t sumFactDist = 0; 
 
	auto prevStop = existingStops.end(); 
	for (const auto& stops : stopBus) 
	{ 
 
		if (auto it = existingStops.find(stops); it != existingStops.end()) 
		{ 
			if (prevStop != existingStops.end()) 
			{ 
				sumDistance += prevStop->second.CalculateDistanceToStop(it->second); 
				if (auto factDistIt = distanceBetweenStops.find({ prevStop->first,stops }); factDistIt != distanceBetweenStops.end()) 
				{ 
					distance = factDistIt->second; 
				} 
				else if (auto factDistRevIt = distanceBetweenStops.find({ stops,prevStop->first }); factDistRevIt != distanceBetweenStops.end()) 
				{ 
					distance = factDistRevIt->second; 
				} 
				sumFactDist += distance; 
			} 
			it->second.AddRoute(numberBus); 
			prevStop = it; 
		} 
	} 
 
	if (!isCircleOrNot) 
	{ 
		prevStop = existingStops.end(); 
		for (const auto& stops : stopBus) 
		{ 
			if (auto it = existingStops.find(stops); it != existingStops.end()) 
			{ 
				if (prevStop != existingStops.end()) 
				{ 
					if (auto factDistIt = distanceBetweenStops.find({ stops,prevStop->first }); factDistIt != distanceBetweenStops.end()) 
					{ 
						distance = factDistIt->second; 
					} 
					else if (auto factDistRevIt = distanceBetweenStops.find({ prevStop->first,stops }); factDistRevIt != distanceBetweenStops.end()) 
					{ 
						distance = factDistRevIt->second; 
					} 
					sumFactDist += distance; 
				} 
				prevStop = it; 
			} 
		} 
	} 
	auto buffer_stopBus = stopBus; 
	sort(buffer_stopBus.begin(), buffer_stopBus.end()); 
	buffer_stopBus.erase(unique(buffer_stopBus.begin(), buffer_stopBus.end()), buffer_stopBus.end()); 
 
	if (isCircleOrNot) 
	{ 
		uniqueStops = buffer_stopBus.size(); 
	} 
	else 
	{ 
		sumDistance *= 2; 
		uniqueStops = buffer_stopBus.size(); 
		stopsOnRoute = stopsOnRoute * 2 - 1; 
	} 
 
	double curvate = sumFactDist / sumDistance; 
 
	return domain::Bus(numberBus, stopBus, stopsOnRoute, uniqueStops, sumFactDist, curvate, isCircleOrNot); 
} 
 
string domain::Bus::GetNumberBus() const 
{ 
	return _numberBus; 
} 
 
size_t domain::Bus::GetStopsOnRoute() const 
{ 
	return _stopsOnRoute; 
} 
 
size_t domain::Bus::GetUniqueStops() const 
{ 
	return _uniqueStops; 
} 
 
size_t domain::Bus::GetSumFactDistance() const 
{ 
	return _sumFactDist; 
} 
double domain::Bus::GetCurvature() const 
{ 
	return _curvate; 
} 
const vector<string>& domain::Bus::GetStopNames() const 
{ 
	return _stopBus; 
} 
 
bool domain::Bus::IsCircleOrNot() const 
{ 
	return _isCircle; 
} 