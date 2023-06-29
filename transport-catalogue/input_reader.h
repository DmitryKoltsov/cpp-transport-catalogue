#pragma once

#include <map>
#include <vector>
#include <string>

#include "transport_catalogue.h"

namespace reader
{
	information::Catalogue ReadData();

	struct QueryBusStops
	{
		std::string _name;
		bool _isBus = true;
	};

	void ReadQueries(std::vector<reader::QueryBusStops>& qbus3);
}