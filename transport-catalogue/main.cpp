#include <iostream>   
#include <sstream>  
#include "json.h"  
#include "transport_catalogue.h"   
#include "json_reader.h"  
#include "router.h"
using namespace std;   
  
int main()   
{   
    information::Catalogue catalogue;  
    graphWorker::Calculate calc;
    JsonReader requests(std::cin);  
    requests.FillCatalogue(catalogue);  
  
    const auto& stat_req = requests.GetStatRequests();  
    const auto& render_settings = requests.GetRenderSettings().AsDict(); 
    const auto& renderer = requests.FillRenderSettings(render_settings);
    const auto& routing_setting = requests.GetRoutingSettings().AsDict();
    const auto& routing = requests.FillRoutingSettings(routing_setting);
    requests.ProcessRequests(stat_req, renderer, routing, catalogue, std::cout);
}