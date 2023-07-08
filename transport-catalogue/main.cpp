#include <iostream> 
#include <sstream>
#include "json.h"
#include "transport_catalogue.h" 
#include "json_reader.h"
using namespace std; 

int main() 
{ 
    information::Catalogue catalogue;
    JsonReader requests(std::cin);
    requests.FillCatalogue(catalogue);

    const auto& stat_req = requests.GetStatRequests();
    const auto& render_settings = requests.GetRenderSettings().AsMap();
    const auto& renderer = requests.FillRenderSettings(render_settings);

    requests.ProcessRequests(stat_req, renderer,catalogue,std::cout);
}