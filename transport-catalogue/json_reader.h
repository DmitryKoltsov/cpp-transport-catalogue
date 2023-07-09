#pragma once 
 
#include "map_renderer.h" 
#include <iostream> 
#include <sstream> 
#include "json_builder.h"
#include "request_handler.h" 
 
class JsonReader 
{ 
public: 
	JsonReader(std::istream& input) : input_(json::Load(input)) 
	{ 
 
	} 
 
	 
 
	const json::Node& GetBaseRequests() const; 
	const json::Node& GetStatRequests() const; 
	const json::Node& GetRenderSettings() const; 
 
	void ProcessRequests(const json::Node& stat_requests, const renderer::MapRenderer& render_settings, information::Catalogue& catalogue, std::ostream& out) const; 
 
	void FillCatalogue(information::Catalogue& catalogue); 
	renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const; 
 
	void PrintRoute(const json::Dict& request_map, const information::Catalogue& catalogue, json::Builder& iBuilder) const;
	void PrintStop(const json::Dict& request_map, const information::Catalogue& catalogue, json::Builder& iBuilder) const;
	void PrintMap(const json::Dict& request_map,const renderer::MapRenderer& render_settings, const information::Catalogue& catalogue, json::Builder& iBuilder) const;
private: 
	json::Document input_; 
	json::Node emp = nullptr; 
}; 