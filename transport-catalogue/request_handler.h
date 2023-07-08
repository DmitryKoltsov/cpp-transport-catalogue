#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace RequestHandler
{
    class RequestHandler
    {
    public:
        explicit RequestHandler(information::Catalogue& catalogue, const renderer::MapRenderer& renderer) : catalogue_(catalogue), renderer_(renderer)
        {

        }

        const std::set<std::string> GetBusesByStop(std::string stop_name) const;

        svg::Document RenderMap(const information::Catalogue& catalogue) const;
    private:
        const information::Catalogue& catalogue_;
        const renderer::MapRenderer& renderer_;
    };
}