#pragma once

#include <set>
#include <map>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "geo.h"
#include <optional>

namespace information
{

	class Stop
	{
	public:

		Stop(double& lat, double& lng, const std::string& stop);

		bool operator<(const Stop& other) const;

		double CalculateDistanceToStop(const Stop& other) const;

		const std::string& GetName() const;

		const std::set<std::string>& GetBusRoute() const;

		void AddRoute(const std::string& numberBus);

	private:
		coordinates::Coordinates _coords;
		std::string _stop;
		std::set<std::string> _busRoutes;
	};

	class Bus
	{
	public:

		Bus(const std::string& numberBus, const std::vector<std::string>& stopBus, size_t stopsOnRoute, size_t uniqueStops, size_t sumFactDist, double curvate);

		bool operator<(const Bus& other) const;

		size_t CurvateLenght();

		static Bus CreateBusRoute(const std::string& numberBus, std::vector<std::string>& stopBus, bool isCircleOrNot, std::map<std::string, Stop>& existingStops, std::map<std::pair<std::string, std::string>, size_t>& distanceBetweenStops);

		std::string GetNumberBus() const;

		size_t GetStopsOnRoute() const;

		size_t GetUniqueStops() const;

		size_t GetSumFactDistance() const;

		double GetCurvature() const;

	private:
		std::string _numberBus;
		std::vector<std::string> _stopBus;
		size_t _stopsOnRoute = 0;
		size_t	_uniqueStops = 0;
		size_t _sumFactDist = 0;
		double _curvate = 0.0;
	};

	class Catalogue
	{
	public:

		void AddStops(const std::string& stop, double lat, double lng, const std::map<std::string, size_t>& distanceToStops);

		void AddBus(const std::string& numberBus, std::vector<std::string>& stopBus, bool isCircleOrNot);

		std::optional<information::Bus> FindBus(const std::string& name) const;

		std::optional<information::Stop> FindStops(const std::string& name) const;

	private:
		std::map<std::string, information::Stop> existingStops;
		std::map<std::pair<std::string, std::string>, size_t> distanceBetweenStops;
		std::map<std::string, information::Bus> existingBus;
	};
}