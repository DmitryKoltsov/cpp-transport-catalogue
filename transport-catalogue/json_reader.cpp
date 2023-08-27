#include "json_reader.h"  
using namespace std::literals;  
  
void JsonReader::PrintInformation(const json::Node& stat_requests, const renderer::MapRenderer& render_settings, information::Catalogue& catalogue, std::ostream& out) const
{
    json::Builder builder;
    auto startFirstArray = builder.StartArray();
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop")
        {
            PrintStop(request_map, catalogue, builder);
        }
        if (type == "Bus")
        {
            PrintRoute(request_map, catalogue, builder);
        }
        if (type == "Map")
        {
            PrintMap(request_map, render_settings, catalogue, builder);
        }
    }
    startFirstArray.EndArray();

    json::Print(json::Document{ builder.Build() }, out);
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, const renderer::MapRenderer& render_settings,const graphWorker::RouterSettings& routing_settings, information::Catalogue& catalogue, std::ostream& out) const
{  
    json::Builder builder; 
    graphWorker::GraphWorker worker(catalogue, routing_settings);
    auto startFirstArray = builder.StartArray(); 
    for (auto& request : stat_requests.AsArray()) {  
        const auto& request_map = request.AsDict();  
        const auto& type = request_map.at("type").AsString();  
        if (type == "Stop")  
        {  
            PrintStop(request_map,catalogue, builder); 
        }
        if (type == "Bus")  
        {  
            PrintRoute(request_map,catalogue, builder); 
        }  
        if (type == "Map")  
        {  
            PrintMap(request_map, render_settings, catalogue, builder); 
        }  
        if (type == "Route")
        {
            PrintRouting(request_map, builder, routing_settings,worker);
        }
    }  
    startFirstArray.EndArray(); 
  
    json::Print(json::Document{ builder.Build() }, out); 
}  

const json::Node& JsonReader::GetBaseRequests() const
{  
	if (!input_.GetRoot().AsDict().count("base_requests")) 
	{  
		return emp;  
	}  
	   
	return input_.GetRoot().AsDict().at("base_requests"); 
}  
  
const json::Node& JsonReader::GetStatRequests() const  
{  
	if (!input_.GetRoot().AsDict().count("stat_requests")) 
	{  
		return emp;  
	}  
  
	return input_.GetRoot().AsDict().at("stat_requests"); 
}  
  
const json::Node& JsonReader::GetRenderSettings() const  
{  
    if (!input_.GetRoot().AsDict().count("render_settings")) 
    {  
        return emp;  
    }  
  
    return input_.GetRoot().AsDict().at("render_settings"); 
}

const json::Node& JsonReader::GetSerializationSettings() const
{
    if (!input_.GetRoot().AsDict().count("serialization_settings"))
    {
        return emp;
    }
    return input_.GetRoot().AsDict().at("serialization_settings").AsDict().at("file");
}

const json::Node& JsonReader::GetRoutingSettings() const
{
    if (!input_.GetRoot().AsDict().count("routing_settings"s))
    {
        return emp;
    }

    return input_.GetRoot().AsDict().at("routing_settings");
}

void JsonReader::FillCatalogue(information::Catalogue& catalogue)  
{  
    const json::Array& arr = GetBaseRequests().AsArray();  
    std::vector<json::Node> buses_request;  
    for (auto& request : arr)  
    {  
        const auto& request_map = request.AsDict(); 
        const auto& type = request_map.at("type").AsString();  
        if (type == "Stop")  
        {  
            const auto& name = request_map.at("name").AsString();  
            const auto& lat = request_map.at("latitude").AsDouble();  
            const auto& lng = request_map.at("longitude").AsDouble();  
            const auto& road = request_map.at("road_distances").AsDict(); 
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
        const auto& requests_bus_map = requests_bus.AsDict(); 
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
  
graphWorker::RouterSettings JsonReader::FillRoutingSettings(const json::Node& settings) const
{
    graphWorker::RouterSettings routing_settings;

    double bus_wait_time = settings.AsDict().at("bus_wait_time"s).AsDouble();
    double bus_velocity = settings.AsDict().at("bus_velocity"s).AsDouble();

    return graphWorker::RouterSettings{ bus_wait_time, bus_velocity };
}

void JsonReader::PrintRoute(const json::Dict& request_map,const information::Catalogue& catalogue, json::Builder& iBuilder) const 
{  
    const std::string& route_number = request_map.at("name").AsString();  
    auto startDict = iBuilder.StartDict(); 
    startDict.Key("request_id").Value(request_map.at("id").AsInt()); 
    auto findBus = catalogue.FindBus(route_number);  
    if (findBus)  
    {  
        startDict.Key("curvature").Value(findBus->GetCurvature()); 
        startDict.Key("route_length").Value(static_cast<int>(findBus->GetSumFactDistance())); 
        startDict.Key("stop_count").Value(static_cast<int>(findBus->GetStopsOnRoute())); 
        startDict.Key("unique_stop_count").Value(static_cast<int>(findBus->GetUniqueStops())); 
    }  
    else  
    {  
        startDict.Key("error_message"s).Value("not found"s); 
    }  
    startDict.EndDict(); 
}  
  
void JsonReader::PrintStop(const json::Dict& request_map, const information::Catalogue& catalogue, json::Builder& iBuilder) const  
{  
    const std::string& stop_name = request_map.at("name").AsString();  
    auto startDict = iBuilder.StartDict(); 
    startDict.Key("request_id").Value(request_map.at("id").AsInt()); 
    auto findStop = catalogue.FindStops(stop_name);  
    if (!findStop)  
    {  
        startDict.Key("error_message"s).Value("not found"s); 
    }  
    else  
    {  
        auto arrayStart = startDict.Key("buses").StartArray(); 
        const std::set<std::string>& busList = findStop->GetBusRoute(); 
        for (auto& bus : busList) 
        {
            arrayStart.Value(bus); 
        } 
        arrayStart.EndArray(); 
    }  
    startDict.EndDict(); 
}  
  
void JsonReader::PrintMap(const json::Dict& request_map,const renderer::MapRenderer& render_settings, const information::Catalogue& catalogue, json::Builder& iBuilder) const 
{  
    auto startDict = iBuilder.StartDict(); 
    startDict.Key("request_id").Value(request_map.at("id").AsInt());
    std::ostringstream strm;
    svg::Document map = render_settings.GetSVG(catalogue.GetExistingBus(), catalogue);
    map.Render(strm);
    startDict.Key("map").Value(strm.str());
 
    startDict.EndDict();
}

void JsonReader::PrintRouting(const json::Dict& request_map, json::Builder& iBuilder,const graphWorker::RouterSettings& routing_settings,graphWorker::GraphWorker& worker) const
{
    const int id = request_map.at("id").AsInt();
    const std::string stop_from = request_map.at("from").AsString();
    const std::string stop_to = request_map.at("to").AsString();
    double total_time = 0.0;
    auto startDict = iBuilder.StartDict();
    auto calc = worker.FindRoute(stop_from,stop_to);
    
    if (!calc.has_value())
    {
        startDict.Key("request_id").Value(request_map.at("id").AsInt());
        startDict.Key("error_message"s).Value("not found"s);
    }
    else
    {

        auto startArray = startDict.Key("items").StartArray();
        for (auto& direction : calc.value())
        {
            auto startDictStop = startArray.StartDict();
            startDictStop.Key("stop_name").Value(direction.stopName);
            startDictStop.Key("time").Value(routing_settings.bus_wait_time_);
            startDictStop.Key("type").Value("Wait"s);
            startDictStop.EndDict();


            auto startDictBus = startArray.StartDict();
            startDictBus.Key("bus").Value(direction.busNumber);
            startDictBus.Key("span_count").Value(direction.span_count);
            startDictBus.Key("time").Value(direction.time - routing_settings.bus_wait_time_);
            startDictBus.Key("type").Value("Bus"s);
            startDictBus.EndDict();


			total_time += direction.time;
        }

        startArray.EndArray();
        startDict.Key("request_id").Value(id);
        startDict.Key("total_time").Value(total_time);
    }
 

    startDict.EndDict();
}