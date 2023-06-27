#pragma once

#include "transport_catalogue.h"

namespace print
{
	void BusInformation(const information::Bus& route);

	void NotFoundBus(const std::string& a);

	void NotFoundStops(const std::string& a);

	void FoundInBusOnRoute(const information::Stop& stop);
}