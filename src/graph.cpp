#include "graph.hpp"

#include <stdint.h>
#include <math.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"
#include "draw.hpp"

#include "logging.h"
#include "my_assert.h"

RendererError Renderer::DrawCoordinatesSystem() {
    Coordinates lt_corner(graph_manager.GetLTCorner());
    Coordinates rb_corner(graph_manager.GetRBCorner());

    LOG(kDebug, "Starting drawing coordinates system");

    sf::RectangleShape rectangle(sf::Vector2f(
        rb_corner.GetCoordinate(0) - lt_corner.GetCoordinate(0),
        rb_corner.GetCoordinate(1) - lt_corner.GetCoordinate(1)
    ));
    rectangle.setPosition(lt_corner.GetCoordinate(0), lt_corner.GetCoordinate(1));
    rectangle.setOutlineColor(sf::Color::Black);
    window.draw(rectangle);

    DrawAxis();

    DrawGraph();

    window.display();

    return kDoneRenderer;
}

RendererError Renderer::DrawAxis() {
    Coordinates lt_corner(graph_manager.GetLTCorner());
    Coordinates rb_corner(graph_manager.GetRBCorner());
    Coordinates start_point(2, lt_corner[0], rb_corner[1]);

    LOG(kDebug, "Starting drawing axis");

    float rb_corner_x = rb_corner.GetCoordinate(0);
    float rb_corner_y = rb_corner.GetCoordinate(1);
    float lt_corner_x = lt_corner.GetCoordinate(0);
    float lt_corner_y = lt_corner.GetCoordinate(1);
    float start_point_x = start_point.GetCoordinate(0);
    float start_point_y = start_point.GetCoordinate(1);

    if ((start_point_x > lt_corner_x) &&
        (start_point_x < rb_corner_x)) {
        sf::RectangleShape axis(sf::Vector2f(kAxisWidth, rb_corner_y - lt_corner_y));
        axis.setPosition(start_point_x, lt_corner_y);
        axis.setFillColor(sf::Color::Blue);
        window.draw(axis);
    }

    if ((start_point_y > lt_corner_y) &&
        (start_point_y < rb_corner_y)) {
        sf::RectangleShape axis(sf::Vector2f(rb_corner_x - lt_corner_x, kAxisWidth));
        axis.setPosition(lt_corner_x, start_point_y - kAxisWidth);
        axis.setFillColor(sf::Color::Blue);
        window.draw(axis);
    }

    sf::RectangleShape line({rb_corner_x - lt_corner_x, kGridWidth});
    sf::Color grid_line_color = sf::Color::Blue;
    grid_line_color.a = kGridBrightness;
    line.setFillColor(grid_line_color);

    float step = (start_point_y - lt_corner_y) / kAxisNum;
    float line_y = start_point_y;
    for (; line_y > lt_corner_y; line_y -= step) {
        line.setPosition({lt_corner_x, line_y});
        window.draw(line);
    }

    line.setSize({kGridWidth, rb_corner_y - lt_corner_y});
    float line_x = start_point_x;
    for (; line_x < rb_corner_x; line_x += step) {
        line.setPosition({line_x, lt_corner_y});
        window.draw(line);
    }

    return kDoneRenderer;
}

RendererError Renderer::DrawGraph() {
    Coordinates lt_corner(graph_manager.GetLTCorner());
    Coordinates rb_corner(graph_manager.GetRBCorner());
    Coordinates start_point(2, lt_corner[0], rb_corner[1]);

    std::vector<float>& points = graph_manager.GetValues();

    float rb_corner_x = rb_corner.GetCoordinate(0);
    float rb_corner_y = rb_corner.GetCoordinate(1);
    float lt_corner_x = lt_corner.GetCoordinate(0);
    float lt_corner_y = lt_corner.GetCoordinate(1);
    float start_point_x = start_point.GetCoordinate(0);
    float start_point_y = start_point.GetCoordinate(1);

    float right_side = rb_corner_x - start_point_x;
    float step = (rb_corner_x - lt_corner_x) / points.size();
    float scale = (lt_corner[1] - start_point[1]) / (graph_manager.GetMaxVal() * kMinDifferenceScale);

    size_t vertex_size = (size_t) floor((double)((rb_corner_x - lt_corner_x) / step)) + 1;
    size_t point_index = 0;

    sf::VertexArray vertices (sf::PrimitiveType::Points, vertex_size);

    size_t val_index = 0;
    size_t length = points.size();
    for (float x = lt_corner_x - start_point_x;
        (x < right_side) && (val_index < length); x += step) {
        float y = points[val_index] * scale;
        val_index++;

        LOG(kDebug, "Y (val) = %f\n", y);

        if ((y > lt_corner_y - start_point_y) &&
            (y < rb_corner_y - start_point_y)) {
            vertices [point_index].position = {x + start_point_x, y + start_point_y};

            vertices [point_index].color = sf::Color::Red;

            point_index++;
        }
    }

    window.draw(vertices);

    return kDoneRenderer;
}
