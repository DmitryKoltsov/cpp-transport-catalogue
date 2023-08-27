#include "transport_catalogue.h"

//#include <iostream>
//#include <map>
//#include <string>
//#include <vector>
//#include <fstream>
#include <filesystem>
#include "map_renderer.h"
#include "transport_router.h"
#include "svg.pb.h"

namespace serialization
{
	struct SerializatorSettings
	{
		std::filesystem::path path;
	};

	class Serializator
	{
	public:

		static void Serialize(information::Catalogue& catalog,const std::string& file_name, const renderer::MapRenderer& renderer,const graphWorker::RouterSettings& route);

		static bool Deserialize(const std::string& file_name, information::Catalogue& catalogue, renderer::RenderSettings& rendererSettings, graphWorker::RouterSettings& route);

		static svg::Color PrColorToColor(const pr_svg::Color& pr_color);
	};
}