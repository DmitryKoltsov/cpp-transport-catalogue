#include "svg.h" 
#include <algorithm> 
#include <string> 
#include <string_view> 
 
namespace svg { 
 
    using namespace std::literals; 
 
    std::ostream& operator<<(std::ostream& out, Color color) { 
        std::visit(ColorPrinter{ out }, color); 
        return out; 
    } 
 
    std::ostream& operator<<(std::ostream& out, const StrokeLineCap linecap) 
    { 
 
        if (linecap == StrokeLineCap::BUTT) { 
            out << "butt"; 
        } 
        else if (linecap == StrokeLineCap::ROUND) { 
            out << "round"; 
        } 
        else if (linecap == StrokeLineCap::SQUARE) { 
            out << "square"; 
        } 
        return out; 
    } 
 
    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin linejoin) 
    { 
        if (linejoin == StrokeLineJoin::ARCS) { 
            out << "arcs"; 
        } 
        else if (linejoin == StrokeLineJoin::BEVEL) { 
            out << "bevel"; 
        } 
        else if (linejoin == StrokeLineJoin::MITER) { 
            out << "miter"; 
        } 
        else if (linejoin == StrokeLineJoin::MITER_CLIP) { 
            out << "miter-clip"; 
        } 
        else if (linejoin == StrokeLineJoin::ROUND) { 
            out << "round"; 
        } 
        return out; 
    } 
 
    void Object::Render(const RenderContext& context) const { 
        context.RenderIndent(); 
 
        RenderObject(context); 
 
        context.out << std::endl; 
    } 
 
 
    Circle& Circle::SetCenter(Point center) { 
        center_ = center; 
        return *this; 
    } 
 
    Circle& Circle::SetRadius(double radius) { 
        radius_ = radius; 
        return *this; 
    } 
 
    void Circle::RenderObject(const RenderContext& context) const { 
        auto& out = context.out; 
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv; 
        out << "r=\""sv << radius_ << "\""sv; 
        RenderAttrs(context.out); 
        out << "/>"sv; 
    } 
 
 
    Polyline& Polyline::AddPoint(Point point) { 
        points_.push_back(std::move(point)); 
        return *this; 
    } 
 
    void Polyline::RenderObject(const RenderContext& context) const { 
        auto& out = context.out; 
        out << "<polyline points=\""; 
        bool is_first = true; 
        for (const Point& point : points_) { 
            if (is_first) { 
                out << point.x << "," << point.y; 
                is_first = false; 
            } 
            else 
            { 
                out << " " << point.x << "," << point.y; 
            } 
        } 
        out << "\""; 
        RenderAttrs(context.out); 
        out << "/>"; 
    } 
 
    Text& Text::SetPosition(Point pos) { 
        pos_ = pos; 
        return *this; 
    } 
 
    Text& Text::SetOffset(Point offset) { 
        offset_ = offset; 
        return *this; 
    } 
 
    Text& Text::SetFontSize(uint32_t size) { 
        font_size_ = size; 
        return *this; 
    } 
 
    Text& Text::SetFontFamily(std::string font_family) { 
        font_family_ = std::move(font_family); 
        return *this; 
    } 
 
    Text& Text::SetFontWeight(std::string font_weight) { 
        font_weight_ = std::move(font_weight); 
        return *this; 
    } 
 
    Text& Text::SetData(std::string data) { 
        data_ = std::move(data); 
        return *this; 
    } 
 
    void Text::RenderObject(const RenderContext& context) const { 
        auto& out = context.out; 
        out << "<text"; 
        RenderAttrs(context.out); 
        out << " x=\"" << pos_.x << "\" y=\"" << pos_.y << "\" "; 
        out << "dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" "; 
        out << "font-size=\"" << font_size_ << "\""; 
 
        if (!font_family_.empty()) { 
            out << " font-family=\"" << font_family_ << "\""; 
        } 
        if (!font_weight_.empty()) { 
            out << " font-weight=\"" << font_weight_ << "\""; 
        } 
        out << ">" << data_ << "</text>"; 
    } 
 
    void Document::AddPtr(std::unique_ptr<Object>&& obj) { 
        objects_.emplace_back(std::move(obj)); 
    } 
 
    void Document::Render(std::ostream& out) const { 
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv; 
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv; 
 
        RenderContext contex(out, 2, 2); 
        for (auto& obj : objects_) { 
            obj->Render(contex); 
        } 
        out << "</svg>"sv; 
    } 
 
} 