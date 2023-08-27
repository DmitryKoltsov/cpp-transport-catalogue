#include "map_renderer.h"  
  
bool renderer::IsZero(double value)  
{  
    return std::abs(value) < EPSILON;  
}  
  
std::vector<svg::Polyline> renderer::MapRenderer::GetRouteLines(const std::map<std::string, domain::Bus>& buses, const SphereProjector& sp,const information::Catalogue& catalogue) const  
{  
    std::vector<svg::Polyline> result;  
    size_t color_num = 0;  
    for (const auto& [bus_number, bus] : buses) {  
        if (bus.GetStopsOnRoute() == 0) continue;  
        std::vector<domain::Stop> route_stops;  
        const std::vector<std::string>& stopNames = bus.GetStopNames();  
        for (const std::string& stopName : stopNames)  
        {  
            std::optional<domain::Stop> stop = catalogue.FindStops(stopName);  
            if (stop.has_value())  
            {  
                route_stops.emplace_back(stop.value());  
            }  
        }  
        svg::Polyline line;  
        for (const auto& stop : route_stops) {  
            line.AddPoint(sp(stop.GetCoords()));  
        }  
        if (!bus.IsCircleOrNot())  
        {  
            for (auto b{ std::next(route_stops.rbegin()) }, e{ route_stops.rend() }; b != e; ++b) {  
                line.AddPoint(sp(b->GetCoords()));  
            }  
        }  
        line.SetStrokeColor(render_settings_.color_palette[color_num]);  
        line.SetFillColor("none");  
        line.SetStrokeWidth(render_settings_.line_width);  
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);  
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);  
  
        if (color_num < (render_settings_.color_palette.size() - 1)) ++color_num;  
        else color_num = 0;  
  
        result.emplace_back(line);  
    }  
  
    return result;  
}  
  
std::vector<svg::Text> renderer::MapRenderer::GetBusLabel(const std::map<std::string, domain::Bus>& buses, const SphereProjector& sp, const information::Catalogue& catalogue) const  
{  
    std::vector<svg::Text> result;  
    size_t color_num = 0;  
  
    auto CreateText{ [this, &sp](const geo::Coordinates& stopCoords, const std::string& busNumber) {  
            svg::Text text;  
            text.SetPosition(sp(stopCoords));  
            text.SetOffset(render_settings_.bus_label_offset);  
            text.SetFontSize(render_settings_.bus_label_font_size);  
            text.SetFontFamily("Verdana");  
            text.SetFontWeight("bold");  
            text.SetData(busNumber);  
  
            return text;  
    } };  
  
    for (const auto& [bus_number, bus] : buses) {  
        auto stops = bus.GetStopNames();  
        if (stops.empty()) continue;  
        auto busNumber = bus.GetNumberBus();  
        auto exStop = catalogue.FindStops(stops.front());  
  
        const auto& stopCoords = exStop->GetCoords();  
  
        svg::Text text{CreateText(stopCoords, busNumber)};  
        text.SetFillColor(render_settings_.color_palette[color_num]);  
        if (color_num < (render_settings_.color_palette.size() - 1)) ++color_num;  
        else color_num = 0;  
        svg::Text underlayer{CreateText(stopCoords, busNumber)};  
        underlayer.SetFillColor(render_settings_.underlayer_color);  
        underlayer.SetStrokeColor(render_settings_.underlayer_color);  
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);  
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);  
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);  
  
        result.emplace_back(underlayer);  
        result.emplace_back(text);  
        if (!bus.IsCircleOrNot() && stops[0] !=stops[stops.size() - 1])  
        {  
            auto exStop = catalogue.FindStops(stops.back());  
            const auto& lastStopCoords = exStop->GetCoords();  
  
            svg::Text text2(text);  
            svg::Text underlayer2(underlayer);  
            text2.SetPosition(sp(lastStopCoords));  
            underlayer2.SetPosition(sp(lastStopCoords));  
  
            result.emplace_back(underlayer2);  
            result.emplace_back(text2);  
        }  
    }  
  
    return result;  
}  
  
std::vector<svg::Circle> renderer::MapRenderer::GetStopsSymbols(const std::map<std::string, domain::Stop>& stops, const SphereProjector& sp) const {  
    std::vector<svg::Circle> result;  
    for (const auto& [stop_name, stop] : stops) {  
        svg::Circle symbol;  
        symbol.SetCenter(sp(stop.GetCoords()));  
        symbol.SetRadius(render_settings_.stop_radius);  
        symbol.SetFillColor("white");  
  
        result.emplace_back(symbol);  
    }  
  
    return result;  
}  
  
std::vector<svg::Text> renderer::MapRenderer::GetStopsLabels(const std::map<std::string, domain::Stop>& stops, const SphereProjector& sp) const  
{  
        std::vector<svg::Text> result;  
        svg::Text text;  
        svg::Text underlayer;  
        for (const auto& [stop_name, stop] : stops) {  
            text.SetPosition(sp(stop.GetCoords()));  
            text.SetOffset(render_settings_.stop_label_offset);  
            text.SetFontSize(render_settings_.stop_label_font_size);  
            text.SetFontFamily("Verdana");  
            text.SetData(stop.GetName());  
            text.SetFillColor("black");  
      
            underlayer.SetPosition(sp(stop.GetCoords()));  
            underlayer.SetOffset(render_settings_.stop_label_offset);  
            underlayer.SetFontSize(render_settings_.stop_label_font_size);  
            underlayer.SetFontFamily("Verdana");  
            underlayer.SetData(stop.GetName());  
            underlayer.SetFillColor(render_settings_.underlayer_color);  
            underlayer.SetStrokeColor(render_settings_.underlayer_color);  
            underlayer.SetStrokeWidth(render_settings_.underlayer_width);  
            underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);  
            underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);  
      
            result.emplace_back(underlayer);  
            result.emplace_back(text);  
        }  
      
        return result;  
}

const renderer::RenderSettings& renderer::MapRenderer::GetRenderSettings() const
{
    return render_settings_;
}
  
svg::Document renderer::MapRenderer::GetSVG(const std::map<std::string, domain::Bus>& buses, const information::Catalogue& catalogue) const  
{  
    svg::Document result;  
    std::vector<geo::Coordinates> route_stops_coords;  
    std::map<std::string, domain::Stop> all_stops;  
  
    for (const auto& [bus_number, bus] : buses) {  
        for (const auto& stopName : bus.GetStopNames()) {  
            if (auto stop = catalogue.FindStops(stopName); stop) {  
                route_stops_coords.emplace_back(stop->GetCoords());  
                all_stops.emplace(stopName, *stop);  
            }  
        }  
    }  
  
    SphereProjector sp(route_stops_coords.begin(), route_stops_coords.end(), render_settings_.width, render_settings_.height, render_settings_.padding);  
    for (const auto& line : GetRouteLines(buses, sp,catalogue))   
    {  
        result.Add(line);  
    }  
    for (const auto& text : GetBusLabel(buses, sp, catalogue))  
    {  
        result.Add(text);  
    }  
    for (const auto& circle : GetStopsSymbols(all_stops, sp))  
    {  
        result.Add(circle);  
    }  
    for (const auto& text : GetStopsLabels(all_stops, sp))  
    {  
        result.Add(text);  
    }  
    return result;  
}