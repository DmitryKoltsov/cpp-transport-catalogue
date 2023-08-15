#pragma once


#include "graph.h"
#include "transport_catalogue.h"


#include <iostream>
#include <map>
#include <string>
#include <memory>

namespace graphWorker
{

	struct RouterSettings
	{
		double bus_wait_time_ = 0.0;
		double bus_velocity_ = 0.0;
	};

	struct Calculate
	{
		std::string stopName;
		std::string busNumber;
		int span_count = 0;
		double time = 0.0;
	};

	class GraphWorker
	{
	public:

		GraphWorker(const information::Catalogue& catalogue, RouterSettings routing_settings);

		std::optional<std::vector<graphWorker::Calculate>> FindRoute(std::string stop1, std::string stop2);
	private:

		struct EdgeInformation
		{
			std::string stopFrom;
			std::string stopTo;
			std::string trueBusNumber;
			double time = 0.0;
			size_t spanCount = 0;
		};

		void FillEdges();

		const information::Catalogue& _catalogue;
		graph::DirectedWeightedGraph<double> _graph;
		std::unique_ptr<graph::Router<double>> _router;
		RouterSettings _routing_settings;

		std::map<graph::EdgeId, EdgeInformation> _edgeIdToBusNumber;
	};
}