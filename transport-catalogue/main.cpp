#include <iostream>   
#include <sstream>  
#include <fstream>
#include "json.h"  
#include "transport_catalogue.h"   
#include "json_reader.h"  
#include "router.h"
#include "serialization.h"
using namespace std;   

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    JsonReader requests(std::cin);

    if (mode == "make_base"sv) 
    {
        information::Catalogue catalogue;
        graphWorker::Calculate calc;

        requests.FillCatalogue(catalogue);
        const auto& render_settings = requests.GetRenderSettings().AsDict();
        const auto& renderer = requests.FillRenderSettings(render_settings);
        const auto& routing_setting = requests.GetRoutingSettings().AsDict();
        const auto& routing = requests.FillRoutingSettings(routing_setting);
        
        const auto file_name = requests.GetSerializationSettings().AsString();
        serialization::Serializator::Serialize(catalogue, file_name, renderer,routing);
    }
    else if (mode == "process_requests"sv) 
    {
        
        const auto file_name = requests.GetSerializationSettings().AsString();

        information::Catalogue catalogue;
        renderer::RenderSettings settings;
        graphWorker::RouterSettings routing_settings;
        if (!serialization::Serializator::Deserialize(file_name, catalogue, settings,routing_settings))
        {
            cout << "bad news..." << endl;
            return 1;
        }
        renderer::MapRenderer renderer(settings);


        const auto& stat_req = requests.GetStatRequests();
        requests.ProcessRequests(stat_req,renderer, routing_settings, catalogue, std::cout);
    }
    else 
    {
        PrintUsage();
        return 1;
    }
    return 0;
}