#include "geo.h"   
#include "transport_catalogue.h"  
using namespace std;   
   
namespace information   
{
	Catalogue::Catalogue(std::map<std::string, domain::Stop>&& stops, std::map<std::pair<std::string, std::string>, size_t>&& distanceBetweenStops, std::map<std::string, domain::Bus>&& buses) :
		_existingStops(std::move(stops)),
		_distanceBetweenStops(std::move(distanceBetweenStops)),
		_existingBus(std::move(buses))
	{
	}

	void Catalogue::AddStops(const string& stop, double lat, double lng, const map<string, int>& distanceToStops)
	{   
		_existingStops.insert(make_pair(stop, domain::Stop(lat, lng, stop)));  
   
		for (const auto& [stopName, stopDistance] : distanceToStops)   
		{   
			_distanceBetweenStops[{stop, stopName}] = stopDistance;   
		}   
	}   
   
	void Catalogue::AddBus(const string& numberBus, vector<string>& stopBus, bool isCircleOrNot)   
	{   
		_existingBus.insert(make_pair(numberBus, domain::Bus::CreateBusRoute(numberBus, stopBus, isCircleOrNot, _existingStops, _distanceBetweenStops)));  
	}   
   
	std::optional<domain::Bus> Catalogue::FindBus(const string& name) const  
	{   
		auto it = _existingBus.find(name);   
   
		if (it != _existingBus.end())   
		{   
			return it->second;   
		}   
		return std::nullopt;   
	}   
   
	std::optional<domain::Stop> Catalogue::FindStops(const string& name) const  
	{   
		auto it = _existingStops.find(name);   
   
		if (it == _existingStops.end())   
		{   
			return std::nullopt;   
		}   
		return it->second;   
	}

	const std::map<std::string, domain::Bus>& Catalogue::GetExistingBus() const  
	{  
		return _existingBus;  
	}
	
	std::map<std::string, domain::Stop> Catalogue::GetExistingStops() const
	{
		return _existingStops;
	}
	int Catalogue::CountStops() const
	{
		return _existingStops.size();
	}

	size_t Catalogue::GetDistance(const std::string stop1, const std::string stop2) const
	{
		if (auto it = _distanceBetweenStops.find({ stop1,stop2 }); it != _distanceBetweenStops.end())
		{
			return it->second;
		}
		else if(auto revWeightIt = _distanceBetweenStops.find({stop2,stop1}); revWeightIt !=_distanceBetweenStops.end())
		{
			return revWeightIt->second;
		}
		return 0;
	}

	const std::map<std::pair<std::string, std::string>, size_t>& Catalogue::GetDistBetweenStops() const
	{
		return _distanceBetweenStops;
	}
}