#include "request_handler.h"

const std::set<std::string> RequestHandler::RequestHandler::GetBusesByStop(std::string stop_name) const
{
    return catalogue_.FindStops(stop_name)->GetBusRoute();
}

svg::Document RequestHandler::RequestHandler::RenderMap(const information::Catalogue& catalogue) const {
    return renderer_.GetSVG(catalogue_.GetExistingBus(), catalogue);
}
