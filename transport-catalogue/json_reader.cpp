#include "json_reader.h"

using namespace std::literals;

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler::RequestHandler& rh, information::Catalogue& catalogue) const
{
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop")
        {
            result.push_back(PrintStop(request_map,rh,catalogue).AsMap());
        }
        if (type == "Bus")
        {
            result.push_back(PrintRoute(request_map,catalogue).AsMap());
        }
        if (type == "Map")
        {
            result.push_back(PrintMap(request_map, rh, catalogue).AsMap());
        }
    }

    json::Print(json::Document{ result }, std::cout);
}

const json::Node& JsonReader::GetBaseRequests() const
{
	if (!input_.GetRoot().AsMap().count("base_requests"))
	{
		return emp;
	}
	 
	return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetStatRequests() const
{
	if (!input_.GetRoot().AsMap().count("stat_requests"))
	{
		return emp;
	}

	return input_.GetRoot().AsMap().at("stat_requests");
}

const json::Node& JsonReader::GetRenderSettings() const
{
    if (!input_.GetRoot().AsMap().count("render_settings"))
    {
        return emp;
    }

    return input_.GetRoot().AsMap().at("render_settings");
}

void JsonReader::FillCatalogue(information::Catalogue& catalogue)
{
    const json::Array& arr = GetBaseRequests().AsArray();
    std::vector<json::Node> buses_request;
    for (auto& request : arr)
    {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop")
        {
            const auto& name = request_map.at("name").AsString();
            const auto& lat = request_map.at("latitude").AsDouble();
            const auto& lng = request_map.at("longitude").AsDouble();
            const auto& road = request_map.at("road_distances").AsMap();
            std::map<std::string, int> buffer_dist;
            for (auto elem : road)
            {
                buffer_dist.emplace(elem.first, elem.second.AsInt());
            }
            catalogue.AddStops(name, lat, lng, buffer_dist);
        }
        else
        {
            buses_request.emplace_back(std::move(request));
        }
    }

    for (auto& requests_bus : buses_request)
    {
        const auto& requests_bus_map = requests_bus.AsMap();
        const auto& type = requests_bus_map.at("type").AsString();
        if (type == "Bus")
        {
            const auto& name = requests_bus_map.at("name").AsString();
            const auto& stop = requests_bus_map.at("stops").AsArray();
            std::vector<std::string> buffer_stops;
            for (auto elem : stop)
            {
                buffer_stops.push_back(elem.AsString());
            }

            const auto& circleOrNot = requests_bus_map.at("is_roundtrip").AsBool();

            catalogue.AddBus(name, buffer_stops, circleOrNot);
        }

    }
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request_map) const
{
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    const auto& underlayer_colors = request_map.at("underlayer_color");
    if (underlayer_colors.IsString()) render_settings.underlayer_color = underlayer_colors.AsString();
    else if (underlayer_colors.IsArray()) {
        const json::Array& underlayer_color = underlayer_colors.AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("wrong underlayer colortype");
    }
    else throw std::logic_error("wrong underlayer color");

    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else throw std::logic_error("wrong color_palette type");
        }
        else throw std::logic_error("wrong color_palette");
    }

    return render_settings;
}

const json::Node JsonReader::PrintRoute(const json::Dict& request_map,const information::Catalogue& catalogue) const
{

    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result.emplace("request_id", request_map.at("id").AsInt());
    auto findBus = catalogue.FindBus(route_number);
    if (findBus)
    {

        result.emplace("curvature", findBus->GetCurvature());
        result.emplace("route_length", static_cast<int>(findBus->GetSumFactDistance()));
        result.emplace("stop_count", static_cast<int>(findBus->GetStopsOnRoute()));
        result.emplace("unique_stop_count", static_cast<int>(findBus->GetUniqueStops()));
    }
    else
    {
        result.emplace("error_message", "not found"s);
    }
    return result;
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler::RequestHandler& rh, const information::Catalogue& catalogue) const
{
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result.emplace("request_id", request_map.at("id").AsInt());
    auto findStop = catalogue.FindStops(stop_name);
    if (!findStop)
    {
        result.emplace("error_message", json::Node{ "not found"s });
    }
    else
    {
        json::Array buses;
        const std::set<std::string>& busList = rh.GetBusesByStop(stop_name);
        for (auto& bus : busList) {
            buses.push_back(bus);
        }
        result.emplace("buses", buses);
    }

    return result;
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler::RequestHandler& rh, const information::Catalogue& catalogue) const
{
    json::Dict result;
    result.emplace("request_id", request_map.at("id").AsInt());
    std::ostringstream strm;
    svg::Document map = rh.RenderMap(catalogue);
    map.Render(strm);
    result.emplace("map", strm.str());

    return result;
}