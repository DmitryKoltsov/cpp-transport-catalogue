#include "serialization.h"
#include "json.h"
#include "transport_catalogue.pb.h"  
#include "map_renderer.pb.h"
#include "transport_router.pb.h"
#include "map_renderer.h"
#include "svg.h"
#include <fstream>
using namespace std;

namespace serialization
{
	void Serializator::Serialize(information::Catalogue& catalog, const string& file_name, const renderer::MapRenderer& renderer, const graphWorker::RouterSettings& route)
	{
		TCProto::TransportCatalogue pb_catalogue;

		//convert to proto buses
		for ( auto elem : catalog.GetExistingBus())
		{
			auto pb_bus = pb_catalogue.add_buses();

			pb_bus->set_name(elem.first);
			for (const auto& stop : elem.second.GetStopNames())
				pb_bus->add_stopbus(stop);
			pb_bus->set_stops_on_route(static_cast<uint32_t>(elem.second.GetStopsOnRoute()));
			pb_bus->set_unique_stops(static_cast<uint32_t>(elem.second.GetUniqueStops()));
			pb_bus->set_sum_fact_dist(static_cast<uint32_t>(elem.second.GetSumFactDistance()));
			pb_bus->set_curvature(elem.second.GetCurvature());
			pb_bus->set_is_roundtrip(elem.second.IsCircleOrNot());
		}

		//conver to proto stops
		for (auto stop : catalog.GetExistingStops())
		{
			auto pb_stops = pb_catalogue.add_stops();

			pb_stops->mutable_coords()->set_lat(stop.second.GetCoords().lat);
			pb_stops->mutable_coords()->set_lng(stop.second.GetCoords().lng);
			pb_stops->set_name(stop.first);
			for (const auto& stop : stop.second.GetBusRoute())
			{
				pb_stops->add_busroutes(stop);
			}
			pb_stops->set_id(static_cast<uint32_t>(stop.second.GetId()));
			
		}

		//convert to proto dist
		for (auto dist : catalog.GetDistBetweenStops())
		{
			auto pb_dist = pb_catalogue.add_distancebetweenstops();

			pb_dist->set_firststop(dist.first.first);
			pb_dist->set_secondstop(dist.first.second);
			pb_dist->set_distance(static_cast<uint32_t>(dist.second));
		}

		//comvert to proto render settings 
		const auto& settings = renderer.GetRenderSettings();
		rend::RenderSettings pr_rend;

		pr_rend.set_width(settings.width);
		pr_rend.set_height(settings.height);
		pr_rend.set_padding(settings.padding);
		pr_rend.set_line_width(settings.line_width);
		pr_rend.set_stop_radius(settings.stop_radius);
		pr_rend.set_bus_label_font_size(settings.bus_label_font_size);
		pr_rend.set_stop_label_font_size(settings.stop_label_font_size);
		pr_rend.set_underlayer_width(settings.underlayer_width);

		pr_rend.mutable_bus_label_offset()->set_x(settings.bus_label_offset.x);
		pr_rend.mutable_bus_label_offset()->set_y(settings.bus_label_offset.y);

		pr_rend.mutable_stop_label_offset()->set_x(settings.stop_label_offset.x);
		pr_rend.mutable_stop_label_offset()->set_y(settings.stop_label_offset.y);
		
		auto colorToPrColor = [](svg::Color color)
		{
			pr_svg::Color pr_color;
			if (std::holds_alternative<std::monostate>(color))
			{
				pr_color.set_monostate(true);
			}
			else if (std::holds_alternative<std::string>(color))
			{
				const std::string& name = std::get<std::string>(color);
				pr_color.set_color_string(name);
			}
			else if (std::holds_alternative<svg::Rgb>(color))
			{
				svg::Rgb rgb = std::get<svg::Rgb>(color);
				pr_color.mutable_color_rgb()->set_red(rgb.red);
				pr_color.mutable_color_rgb()->set_green(rgb.green);
				pr_color.mutable_color_rgb()->set_blue(rgb.blue);
			}
			else
			{
				svg::Rgba rgba = std::get<svg::Rgba>(color);
				pr_color.mutable_color_rgba()->set_red(rgba.red);
				pr_color.mutable_color_rgba()->set_green(rgba.green);
				pr_color.mutable_color_rgba()->set_blue(rgba.blue);
				pr_color.mutable_color_rgba()->set_opacity(rgba.opacity);
			}

			return pr_color;
		};
		 
		*pr_rend.mutable_underlayer_color() = colorToPrColor(settings.underlayer_color);

		for (auto elem : settings.color_palette)
		{
			*pr_rend.add_color_palette() = colorToPrColor(elem);
		}
				
		pr_route::RouterSettings pr_route_settings;
		pr_route_settings.set_bus_velocity(route.bus_velocity_);
		pr_route_settings.set_bus_wait_time(route.bus_wait_time_);

		TCProto::TransportData transportData;

		*transportData.mutable_transportcatalogue() = pb_catalogue;
		*transportData.mutable_rendersettings() = pr_rend;
		*transportData.mutable_routersettings() = pr_route_settings;

		std::ofstream out{file_name, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc};
		transportData.SerializeToOstream(&out);
	}
	
	bool Serializator::Deserialize(const string& file_name,information::Catalogue& catalogue, renderer::RenderSettings& rendererSettings, graphWorker::RouterSettings& route)
	{
		//convert from proto to catalog

		std::ifstream in{file_name, std::ios_base::in | std::ios_base::binary};

		// Прочитать данные из pb файла
		TCProto::TransportData transportData;
		if (!transportData.ParseFromIstream(&in))
		{
			return false;
		}
		TCProto::TransportCatalogue pb_catalogue = transportData.transportcatalogue();
		// Получить вектор автобусов из pb
		std::vector<TCProto::Bus> pb_buses(pb_catalogue.buses().begin(), pb_catalogue.buses().end());

		// Распаковать автобусы из pb и добавить их в каталог
		std::map<std::string, domain::Bus> existingBus;
		for (const auto& pb_bus : pb_buses)
		{
			vector<string> stopBus;
			for (const auto& stop : pb_bus.stopbus())
			{
				stopBus.push_back(stop);
			}

			existingBus.emplace(make_pair(pb_bus.name(), domain::Bus(pb_bus.name(), stopBus, pb_bus.stops_on_route(), pb_bus.unique_stops(), pb_bus.sum_fact_dist(), pb_bus.curvature(), pb_bus.is_roundtrip())));

		}

		// Получить вектор расстояний между остановками из pb
		std::vector<TCProto::Distance> pb_distances(pb_catalogue.distancebetweenstops().begin(), pb_catalogue.distancebetweenstops().end());
			
		// Распаковать расстояния из pb и добавить их в каталог
		// 
		// Получить вектор остановок из pb
		std::map<std::pair<std::string, std::string>, size_t> distanceBetweenStops;
		for (const auto& pb_distance : pb_distances)
		{
			auto firstStop = pb_distance.firststop();
			auto secondStop = pb_distance.secondstop();
			auto distance = pb_distance.distance();
			distanceBetweenStops.emplace(make_pair(make_pair(pb_distance.firststop(), pb_distance.secondstop()), pb_distance.distance()));
		}

		std::vector<TCProto::Stop> pb_stops(pb_catalogue.stops().begin(), pb_catalogue.stops().end());

		std::map<std::string, domain::Stop> existingStops;
		for (const auto& pb_stop : pb_stops)
		{
			auto lat = pb_stop.coords().lat();
			auto lng = pb_stop.coords().lng();
			auto id = pb_stop.id();
			domain::Stop stop(lat, lng, pb_stop.name());
			for (const auto& busName : pb_stop.busroutes())
			{
				stop.AddRoute(busName);
			}
			existingStops.emplace(make_pair(pb_stop.name(), std::move(stop)));
		}

		catalogue = information::Catalogue(std::move(existingStops), std::move(distanceBetweenStops), std::move(existingBus));

		//conver PrColorToColor
	
		//conver PrRenderToRender
		rend::RenderSettings pr_rend = transportData.rendersettings();
		rendererSettings.width = pr_rend.width();
		rendererSettings.height = pr_rend.height();
		rendererSettings.padding = pr_rend.padding();
		rendererSettings.line_width = pr_rend.line_width();
		rendererSettings.stop_radius = pr_rend.stop_radius();
		rendererSettings.bus_label_font_size = pr_rend.bus_label_font_size();

		const pr_svg::Point& bus_offset = pr_rend.bus_label_offset();
		rendererSettings.bus_label_offset.x = bus_offset.x();
		rendererSettings.bus_label_offset.y = bus_offset.y();

		rendererSettings.stop_label_font_size = pr_rend.stop_label_font_size();

		const pr_svg::Point& stop_offset = pr_rend.stop_label_offset();
		rendererSettings.stop_label_offset.x = stop_offset.x();
		rendererSettings.stop_label_offset.y = stop_offset.y();

		rendererSettings.underlayer_color = PrColorToColor(pr_rend.underlayer_color());

		rendererSettings.underlayer_width = pr_rend.underlayer_width();

		size_t color_palette_size = pr_rend.color_palette_size();
		rendererSettings.color_palette.reserve(color_palette_size);

		for (int i = 0; i < color_palette_size; ++i)
		{
			rendererSettings.color_palette.emplace_back(PrColorToColor(pr_rend.color_palette(i)));
		}

		pr_route::RouterSettings pr_rs = transportData.routersettings();
		route.bus_velocity_ = pr_rs.bus_velocity();
		route.bus_wait_time_ = pr_rs.bus_wait_time();


		return true;
	}

	svg::Color Serializator::PrColorToColor(const pr_svg::Color& pr_color)
	{
		if (pr_color.has_color_rgb())
		{
			return svg::Color{ svg::Rgb{static_cast<uint8_t>(pr_color.color_rgb().red()), static_cast<uint8_t>(pr_color.color_rgb().green()), static_cast<uint8_t>(pr_color.color_rgb().blue())} };
		}
		else if (pr_color.has_color_rgba())
		{
			return svg::Color{ svg::Rgba{static_cast<uint8_t>(pr_color.color_rgba().red()), static_cast<uint8_t>(pr_color.color_rgba().green()), static_cast<uint8_t>(pr_color.color_rgba().blue()), pr_color.color_rgba().opacity()} };
		}
		else
		{
			return svg::Color{ pr_color.color_string() };
		}
		return svg::Color{};
	}

}