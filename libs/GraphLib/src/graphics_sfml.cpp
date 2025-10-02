#include "graphics.hpp"

#include <stdlib.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"

namespace graphics {

//-----------------VERTEX ARRAY-------------------------------------------------------------------------------

    VertexArray::VertexArray(size_t size) {
        vertex_array_ = new(std::nothrow) sf::VertexArray(sf::PrimitiveType::Points, size);
        if (vertex_array_ == NULL) {
            throw std::runtime_error("Can't open a window\n");
        }
    }

    VertexArray::~VertexArray() {
        // delete ((sf::VertexArray*)(vertex_array_));
    }

//-----------------RECTANGLE SHAPE----------------------------------------------------------------------------

    RectangleShape::RectangleShape(float width, float height) {
        rectangle_ = new(std::nothrow) sf::RectangleShape(sf::Vector2f(width, height));
        if (rectangle_ == NULL) {
            throw std::runtime_error("Can't open a window\n");
        }
    }

    RectangleShape::~RectangleShape() {
        delete ((sf::RectangleShape*)(rectangle_));
    }

    void RectangleShape::SetSize(float width, float height) const {
        ((sf::RectangleShape*)(rectangle_))->setSize({width, height});
    }

    void RectangleShape::SetPosition(const Coordinates& lt_corner) const {
        ((sf::RectangleShape*)(rectangle_))->setPosition(lt_corner[0], lt_corner[1]);
    }

    void RectangleShape::SetRotation(float angle) const {
        ((sf::RectangleShape*)(rectangle_))->setRotation(angle);
    }
    void RectangleShape::Rotate(float angle) const {
        ((sf::RectangleShape*)(rectangle_))->rotate(angle);
    }

    void RectangleShape::SetOutlineColor(const graphics::Color color) const {
        ((sf::RectangleShape*)(rectangle_))->setOutlineColor(
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness()));
    }
    void RectangleShape::SetFillColor(const graphics::Color color) const {
        ((sf::RectangleShape*)(rectangle_))->setFillColor(
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness()));
    }

//-----------------RENDER WINDOW------------------------------------------------------------------------------

    RenderWindow::RenderWindow(size_t width, size_t height, const char* window_name) {
        width_ = width;
        height_ = height;
        window_ = new(std::nothrow) sf::RenderWindow(sf::VideoMode(width, height), window_name);
        if (window_ == NULL) {
            throw std::runtime_error("Can't open a window\n");
        }
    }

    RenderWindow::~RenderWindow() {
        delete ((sf::RenderWindow*)(window_));
    }

    bool RenderWindow::PollEvent(sf::Event& event) const {
        return ((sf::RenderWindow*)(window_))->pollEvent(event);
    }

    float RenderWindow::GetWidth() const {
        return ((sf::RenderWindow*)(window_))->getSize().x;
    };

    float RenderWindow::GetHeight() const {
        return ((sf::RenderWindow*)(window_))->getSize().y;
    };

    Coordinates RenderWindow::GetMousePos() const {
        float scale_x = GetWidth() / width_;
        float scale_y = GetHeight() / height_;
        sf::RenderWindow* render = (sf::RenderWindow*)window_;
        return Coordinates(2, (float)sf::Mouse::getPosition(*render).x / scale_x,
                              (float)sf::Mouse::getPosition(*render).y / scale_y);
    }

    void RenderWindow::Draw(const graphics::RectangleShape& rect) const {
        ((sf::RenderWindow*)(window_))->draw(*((sf::RectangleShape*)rect.GetRectangle()));
    }
    void RenderWindow::Draw(const sf::VertexArray& arr) const {
        ((sf::RenderWindow*)(window_))->draw(arr);
    }
    void RenderWindow::Draw(const sf::Text& text) const {
        ((sf::RenderWindow*)(window_))->draw(text);
    }

    void RenderWindow::Display() const {
        ((sf::RenderWindow*)(window_))->display();
    }

    bool RenderWindow::IsOpen() const {
        return ((sf::RenderWindow*)(window_))->isOpen();
    }

    void RenderWindow::Close() const {
        ((sf::RenderWindow*)(window_))->close();
    }

//------------------------------------------------------------------------------------------------------------

}
