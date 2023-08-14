#include "request_handler.h" 

using namespace std;

graphWorker::GraphWorker::GraphWorker(const information::Catalogue& catalogue, RouterSettings routing_settings) :
	_catalogue(catalogue),
	_graph(static_cast<size_t>(catalogue.CountStops())),
	_routing_settings(routing_settings)
{
	FillEdges();
	_router = std::make_unique<graph::Router<double>>(_graph);
}

void graphWorker::GraphWorker::FillEdges()
{
	const auto& existStops = _catalogue.GetExistingStops();
	const auto& existBus = _catalogue.GetExistingBus();

	const auto busSpeed = 0.06 / _routing_settings.bus_velocity_;
	for (const auto& bus : existBus)
	{
		const auto& stop = bus.second.GetStopNames();
		if (bus.second.IsCircleOrNot())
		{
			for (size_t i = 0; i < stop.size(); i++)
			{
				const auto& stop1 = stop[i];
				
				auto routeTime = 0.0;
				for (size_t j = i + 1; j < stop.size(); j++)
				{
					if (i == 0 && j == stop.size() - 1)
						continue;

					const auto& stop2 = stop[j];
					const auto& prevStop = stop[j - 1];

					routeTime += _catalogue.GetDistance(prevStop, stop2) * busSpeed;

					graph::Edge<double> edge;
					auto fromId = existStops.find(stop1);
					auto toId = existStops.find(stop2);
					edge.from = fromId->second.GetId();
					edge.to = toId->second.GetId();
					edge.weight = routeTime + _routing_settings.bus_wait_time_;
					auto edgeId = _graph.AddEdge(edge);

					EdgeInformation info;
					info.trueBusNumber = bus.first;
					info.stopFrom = stop1;
					info.stopTo = stop2;
					info.time = edge.weight;
					info.spanCount = j - i;

					_edgeIdToBusNumber.emplace(edgeId, info);
				}
			}
		}
		else
		{
			for (size_t i = 0; i < stop.size(); i++)
			{
				const auto& stop1 = stop[i];
				auto routeTime = 0.0;
				auto revRouteTime = 0.0;
				for (size_t j = i + 1; j < stop.size(); j++)
				{
					const auto& stop2 = stop[j];
					const auto& prevStop = stop[j - 1];

					auto fromId = existStops.find(stop1);
					auto toId = existStops.find(stop2);

					{
						routeTime += _catalogue.GetDistance(prevStop, stop2) * busSpeed;

						graph::Edge<double> edge;
						edge.from = fromId->second.GetId();
						edge.to = toId->second.GetId();
						edge.weight = routeTime + _routing_settings.bus_wait_time_;
						auto edgeId = _graph.AddEdge(edge);

						EdgeInformation info;
						info.trueBusNumber = bus.first;
						info.stopFrom = stop1;
						info.stopTo = stop2;
						info.time = edge.weight;
						info.spanCount = j - i;
						_edgeIdToBusNumber.emplace(edgeId, info);
					}

					{
						revRouteTime += _catalogue.GetDistance(stop2, prevStop) * busSpeed;

						graph::Edge<double> revEdge;
						revEdge.from = toId->second.GetId();
						revEdge.to = fromId->second.GetId();
						revEdge.weight = revRouteTime + _routing_settings.bus_wait_time_;
						auto revEdgeId = _graph.AddEdge(revEdge);
	
						EdgeInformation revInfo;
						revInfo.trueBusNumber = bus.first;
						revInfo.stopFrom = stop2;
						revInfo.stopTo = stop1;
						revInfo.time = revEdge.weight;
						revInfo.spanCount = j - i;
						_edgeIdToBusNumber.emplace(revEdgeId, revInfo);
					}
				}
			}
		}
		
	}

	
}

std::optional<std::vector<graphWorker::Calculate>> graphWorker::GraphWorker::FindRoute(std::string stop1, std::string stop2)
{
	const auto& existStops = _catalogue.GetExistingStops();

	auto fromId = existStops.find(stop1);
	auto toId = existStops.find(stop2);

	if (fromId == existStops.end())
	{
		return std::nullopt;
	}
	if (toId == existStops.end())
	{
		return std::nullopt;
	}

	auto best_route = _router->BuildRoute(fromId->second.GetId(),toId->second.GetId());
	if (!best_route.has_value())
	{
		return std::nullopt;
	}

	vector <graphWorker::Calculate> busNumberToSpan;
	vector<string> prevBusName;
	for (const auto& edge : best_route->edges)
	{
		auto needStops = _edgeIdToBusNumber.find(edge);
		Calculate prevOperation;
		prevOperation.span_count = needStops->second.spanCount;
		prevOperation.busNumber = needStops->second.trueBusNumber;
		prevOperation.stopName = needStops->second.stopFrom;
		prevOperation.time = needStops->second.time;
			
		busNumberToSpan.push_back(prevOperation);

	}
	return busNumberToSpan;

}
