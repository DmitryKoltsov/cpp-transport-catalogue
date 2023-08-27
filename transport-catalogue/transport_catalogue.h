#pragma once   
   
#include <set>   
#include <map>   
#include <algorithm>   
#include <iostream>   
#include <string>   
#include <vector>   
#include "geo.h"   
#include <optional>   
#include "svg.h"  
#include "domain.h"  
#include "router.h"
  
namespace information   
{   
	class Catalogue   
	{   
	public:   

		Catalogue() = default;

		Catalogue(std::map<std::string, domain::Stop>&& stops, std::map<std::pair<std::string, std::string>, size_t>&& distanceBetweenStops, std::map<std::string, domain::Bus>&& buses);
  
		void AddStops(const std::string& stop, double lat, double lng, const std::map<std::string, int>& distanceToStops);  
   
		void AddBus(const std::string& numberBus, std::vector<std::string>& stopBus, bool isCircleOrNot);   
   
		std::optional<domain::Bus> FindBus(const std::string& name) const;  
   
		std::optional<domain::Stop> FindStops(const std::string& name) const;  

		const std::map<std::string, domain::Bus>& GetExistingBus() const;  

		std::map<std::string, domain::Stop> GetExistingStops() const;

		int CountStops() const;

		size_t GetDistance(const std::string stop1, const std::string stop2) const;

		const std::map < std::pair < std::string,std::string > , size_t>& GetDistBetweenStops() const;

	private:   
		std::map<std::string, domain::Stop> _existingStops;  
		std::map<std::pair<std::string, std::string>, size_t>  _distanceBetweenStops;   
		std::map<std::string, domain::Bus> _existingBus;  
	};   
}