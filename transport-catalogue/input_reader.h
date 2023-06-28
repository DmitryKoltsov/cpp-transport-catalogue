#pragma once

#include <map>
#include <vector>
#include <string>

namespace reader
{

	struct Stop
	{
		double _lat = 0.0;
		double _lng = 0.0;
		std::string _stop;
		std::map<std::string, size_t> _distanceToStops;
	};

	struct Bus
	{
		std::string _numberBus;
		std::vector<std::string> _stopBus;
		bool _isCircleOrNot = true;
	};

	struct QueryBusStops
	{
		std::string _name;
		bool _isBus = true;
	};

	QueryBusStops GetFindNumberBus(std::string a);

	void ReadData(std::vector<reader::Stop>& stop3, std::vector<reader::Bus>& bus3);

	void ReadQueries(std::vector<reader::QueryBusStops>& qbus3);
}