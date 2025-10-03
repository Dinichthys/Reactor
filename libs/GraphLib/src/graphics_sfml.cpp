#include "graphics.hpp"

#include <stdlib.h>
#include <stdexcept>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"

namespace graphics {

//-----------------TEXT---------------------------------------------------------------------------------------

    Text::Text(const char* text, const char* font_file_name, unsigned char height) {
        font_ = new(std::nothrow) sf::Font();
        if (font_ == NULL) {
            throw std::runtime_error("Can't create font for Text object\n");
        }

        if (!((sf::Font*)font_)->loadFromFile(font_file_name)) {
            throw std::runtime_error("Can't cload font\n");
        }

        text_ = new(std::nothrow) sf::Text(text, *((sf::Font*)font_), height);
        if (text_ == NULL) {
            throw std::runtime_error("Can't create Text object\n");
        }
    }

    Text::Text(const Text& other) {
        font_ = new(std::nothrow) sf::Font(*((sf::Font*)other.GetFont()));
        if (font_ == NULL) {
            throw std::runtime_error("Can't create font for Text object\n");
        }

        text_ = new(std::nothrow) sf::Text(*((sf::Text*)other.GetText()));
        if (text_ == NULL) {
            throw std::runtime_error("Can't create Text object\n");
        }

        ((sf::Text*)text_)->setFont(*((sf::Font*)font_));
    }

    Text::~Text() {
        delete ((sf::Text*)text_);
        delete ((sf::Font*)font_);
    }

    void Text::SetPosition(const Coordinates& lt_corner) const {
        ((sf::Text*)text_)->setPosition({lt_corner[0], lt_corner[1]});
    }

//-----------------VERTEX ARRAY-------------------------------------------------------------------------------

    VertexArray::VertexArray(size_t size) {
        vertex_array_ = new(std::nothrow) sf::VertexArray(sf::PrimitiveType::Points, size);
        if (vertex_array_ == NULL) {
            throw std::runtime_error("Can't create VertexArray\n");
        }
    }

    VertexArray::VertexArray(const VertexArray& other) {
        vertex_array_ = new(std::nothrow) sf::VertexArray(*((sf::VertexArray*)other.GetVertexArray()));
        if (vertex_array_ == NULL) {
            throw std::runtime_error("Can't create VertexArray\n");
        }
    }

    VertexArray::~VertexArray() {
        delete ((sf::VertexArray*)vertex_array_);
    }

    void VertexArray::SetPixelPosition(size_t index, const Coordinates& pos) const {
        (*((sf::VertexArray*)vertex_array_))[index].position = {pos[0], pos[1]};
    }
    void VertexArray::SetPixelColor(size_t index, const Color& color) const {
        (*((sf::VertexArray*)vertex_array_))[index].color =
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness());
    }

//-----------------RECTANGLE SHAPE----------------------------------------------------------------------------

    RectangleShape::RectangleShape(float width, float height) {
        rectangle_ = new(std::nothrow) sf::RectangleShape(sf::Vector2f(width, height));
        if (rectangle_ == NULL) {
            throw std::runtime_error("Can't create RectangleShape\n");
        }
    }

    RectangleShape::RectangleShape(const RectangleShape& other) {
        rectangle_ = new(std::nothrow) sf::RectangleShape(*((sf::RectangleShape*)other.GetRectangle()));
        if (rectangle_ == NULL) {
            throw std::runtime_error("Can't create RectangleShape\n");
        }
    }

    RectangleShape::~RectangleShape() {
        delete ((sf::RectangleShape*)rectangle_);
    }

    void RectangleShape::SetSize(float width, float height) const {
        ((sf::RectangleShape*)rectangle_)->setSize({width, height});
    }

    void RectangleShape::SetPosition(const Coordinates& lt_corner) const {
        ((sf::RectangleShape*)rectangle_)->setPosition(lt_corner[0], lt_corner[1]);
    }

    void RectangleShape::SetRotation(float angle) const {
        ((sf::RectangleShape*)rectangle_)->setRotation(angle);
    }
    void RectangleShape::Rotate(float angle) const {
        ((sf::RectangleShape*)rectangle_)->rotate(angle);
    }

    void RectangleShape::SetOutlineColor(const graphics::Color color) const {
        ((sf::RectangleShape*)rectangle_)->setOutlineColor(
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness()));
    }
    void RectangleShape::SetFillColor(const graphics::Color color) const {
        ((sf::RectangleShape*)rectangle_)->setFillColor(
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
        delete ((sf::RenderWindow*)window_);
    }

    bool RenderWindow::PollEvent(graphics::Event& event) const {
        sf::Event sf_event;
        if (!(((sf::RenderWindow*)window_)->pollEvent(sf_event))) {
            return false;
        }

        switch (sf_event.type) {
            case sf::Event::Closed : {
                event.SetType(graphics::kClosed);
                break;
            }
            case sf::Event::MouseButtonPressed : {
                event.SetType(graphics::kMouseButtonPressed);
                break;
            }
            case sf::Event::MouseButtonReleased : {
                event.SetType(graphics::kMouseButtonReleased);
                break;
            }
            case sf::Event::MouseMoved : {
                event.SetType(graphics::kMouseMoved);
                break;
            }
            default : {
                event.SetType(graphics::kNoneEvent);
                break;
            }
        }

        return true;
    }

    float RenderWindow::GetWidth() const {
        return ((sf::RenderWindow*)window_)->getSize().x;
    };

    float RenderWindow::GetHeight() const {
        return ((sf::RenderWindow*)window_)->getSize().y;
    };

    Coordinates RenderWindow::GetMousePos() const {
        float scale_x = GetWidth() / width_;
        float scale_y = GetHeight() / height_;
        sf::RenderWindow* render = (sf::RenderWindow*)window_;
        return Coordinates(2, (float)sf::Mouse::getPosition(*render).x / scale_x,
                              (float)sf::Mouse::getPosition(*render).y / scale_y);
    }

    void RenderWindow::Draw(const graphics::RectangleShape& rect) const {
        ((sf::RenderWindow*)window_)->draw(*((sf::RectangleShape*)rect.GetRectangle()));
    }
    void RenderWindow::Draw(const graphics::VertexArray& arr) const {
        ((sf::RenderWindow*)window_)->draw(*((sf::VertexArray*)arr.GetVertexArray()));
    }
    void RenderWindow::Draw(const graphics::Text& text) const {
        ((sf::RenderWindow*)window_)->draw(*((sf::Text*)text.GetText()));
    }

    void RenderWindow::Display() const {
        ((sf::RenderWindow*)window_)->display();
    }

    bool RenderWindow::IsOpen() const {
        return ((sf::RenderWindow*)window_)->isOpen();
    }

    void RenderWindow::Close() const {
        ((sf::RenderWindow*)window_)->close();
    }

//------------------------------------------------------------------------------------------------------------

}
