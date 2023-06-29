#pragma once

#include "transport_catalogue.h"

namespace print
{
	void ReadAndExecuteQueries(const information::Catalogue& catalogue);

	void BusInformation(std::ostream& stream, const information::Bus& route);

	void NotFoundBus(std::ostream& stream, const std::string& a);

	void NotFoundStops(std::ostream& stream, const std::string& a);

	void FoundInBusOnRoute(std::ostream& stream, const information::Stop& stop);
}