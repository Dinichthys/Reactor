#include "graph.hpp"

#include <stdint.h>
#include <math.h>

#include "graphics.hpp"

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"
#include "draw.hpp"

#include "logging.h"
#include "my_assert.h"

void GraphManager::Draw(graphics::RenderWindow* window) {
    ASSERT(window != NULL, "");

    Coordinates lt_corner(Widget::GetLTCornerAbs());
    float width = Widget::GetWidth();
    float height = Widget::GetHeight();

    LOG(kDebug, "Starting drawing coordinates system");

    graphics::RectangleShape rectangle(width, height);
    rectangle.SetPosition(lt_corner);
    rectangle.SetOutlineColor(graphics::kColorBlack);
    window->Draw(rectangle);

    DrawAxis(window);

    DrawGraph(window);
}

GraphError GraphManager::DrawAxis(graphics::RenderWindow* window) {
    ASSERT(window != NULL, "");

    Coordinates lt_corner(Widget::GetLTCornerAbs());
    Coordinates rb_corner(Widget::GetRBCornerAbs());

    LOG(kDebug, "Starting drawing axis");

    float rb_corner_x = rb_corner.GetCoordinate(0);
    float rb_corner_y = rb_corner.GetCoordinate(1);
    float lt_corner_x = lt_corner.GetCoordinate(0);
    float lt_corner_y = lt_corner.GetCoordinate(1);

    float step = (rb_corner_y - lt_corner_y) / kAxisNum;
    float start_point_x = lt_corner_x + step;
    float start_point_y = rb_corner_y - step;

    graphics::RectangleShape line(rb_corner_x - lt_corner_x, kGridWidth);
    graphics::Color grid_line_color = graphics::kColorBlue;
    grid_line_color.SetBrightness(kGridBrightness);
    line.SetFillColor(grid_line_color);

    float line_y = start_point_y;
    for (; line_y > lt_corner_y; line_y -= step) {
        line.SetPosition(Coordinates(2, lt_corner_x, line_y));
        window->Draw(line);
    }

    line.SetSize(kGridWidth, rb_corner_y - lt_corner_y);
    float line_x = start_point_x;
    for (; line_x < rb_corner_x; line_x += step) {
        line.SetPosition(Coordinates(2, line_x, lt_corner_y));
        window->Draw(line);
    }

    grid_line_color.SetBrightness(kMaxBrightness);
    line.SetFillColor(grid_line_color);
    if ((start_point_x > lt_corner_x) &&
        (start_point_x < rb_corner_x)) {
        line.SetSize(kAxisWidth, rb_corner_y - lt_corner_y);
        line.SetPosition(Coordinates(2, start_point_x, lt_corner_y));
        line.SetFillColor(graphics::kColorBlue);
        window->Draw(line);
    }

    line.SetSize(kAxisWidth, kArrowLenPercentage * (rb_corner_y - lt_corner_y));
    line.Rotate(kArrowAngle);
    window->Draw(line);
    line.Rotate(360 - 2 * kArrowAngle);
    window->Draw(line);

    if ((start_point_y > lt_corner_y) &&
        (start_point_y < rb_corner_y)) {
        line.SetRotation(0);
        line.SetSize(rb_corner_x - lt_corner_x, kAxisWidth);
        line.SetPosition(Coordinates(2, lt_corner_x, start_point_y - kAxisWidth));
        line.SetFillColor(graphics::kColorBlue);
        window->Draw(line);
    }

    line.SetPosition(Coordinates(2, rb_corner_x, start_point_y));
    line.SetSize(kArrowLenPercentage * (rb_corner_x - lt_corner_x), kAxisWidth);
    line.Rotate(180 - kArrowAngle);
    window->Draw(line);
    line.Rotate(2 * kArrowAngle);
    window->Draw(line);

    return kDoneGraph;
}

GraphError GraphManager::DrawGraph(graphics::RenderWindow* window) {
    ASSERT(window != NULL, "");

    Coordinates lt_corner(Widget::GetLTCornerAbs());
    Coordinates rb_corner(Widget::GetRBCornerAbs());

    float rb_corner_x = rb_corner.GetCoordinate(0);
    float rb_corner_y = rb_corner.GetCoordinate(1);
    float lt_corner_x = lt_corner.GetCoordinate(0);
    float lt_corner_y = lt_corner.GetCoordinate(1);

    float step_axis = (rb_corner_y - lt_corner_y) / kAxisNum;
    float start_point_x = lt_corner_x + step_axis;
    float start_point_y = rb_corner_y - step_axis;

    float step = (rb_corner_x - start_point_x) / points.size();
    float scale = (lt_corner_y - start_point_y) / (max_ * kMinDifferenceScale);

    size_t vertex_size = points.size();
    size_t point_index = 0;

    sf::VertexArray vertices (sf::PrimitiveType::Points, vertex_size);

    size_t val_index = 0;
    for (float x = start_point_x;
        (x < rb_corner_x) && (val_index < vertex_size); x += step) {
        float y = points[val_index] * scale;
        val_index++;

        if ((y > lt_corner_y - start_point_y) &&
            (y < rb_corner_y - start_point_y)) {
            vertices [point_index].position = {x, y + start_point_y};

            vertices [point_index].color = sf::Color::Red;

            point_index++;
        }
    }

    window->Draw(vertices);

    return kDoneGraph;
}
