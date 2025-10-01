#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>

#include "reactor.hpp"
#include "window.hpp"
#include "my_assert.h"
#include "text.hpp"

static const sf::Color kDefaultButtonColor = sf::Color::Blue;
static const sf::Color kPressedColor = sf::Color::Green;
static const sf::Color kReleaseColor = sf::Color::Red;
static const sf::Color kPanelColor = sf::Color::White;

class Button : public WidgetContainer {
    private:
        bool pressed;

    public:
        explicit Button(const Button& other)
            :WidgetContainer(other) {
            pressed = other.GetPressedInfo();

            Text* text = dynamic_cast<Text*>(other.GetChild(0));
            if (text == NULL) {
                return;
            }

            WidgetContainer::SetChild(0, new(std::nothrow) Text(*text));
            if (WidgetContainer::GetChild(0) == NULL) {
                throw std::runtime_error("Can't do copy constructor for Button\n");
            }
        };

        explicit Button(const Coordinates& lt_corner, float width, float height,
                        const char* text = NULL, const char* file_name = NULL, Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height) {
            pressed = false;

            if ((text != NULL) && (file_name != NULL)) {
                WidgetContainer::AddChild(new(std::nothrow) Text(Coordinates(2, 0, 0), width, height, this, text, file_name));
                if (WidgetContainer::GetChildren().back() == NULL) {
                    throw std::runtime_error("Bad allocation for text");
                }
            }

            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        bool GetPressedInfo() const {return pressed;};
        void SetPressedInfo(bool new_pressed) {pressed = new_pressed;};

        virtual void Draw(sf::RenderWindow* window) {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            sf::RectangleShape button_background(sf::Vector2f(width, height));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            if (Button::GetPressedInfo()) {
                button_background.setFillColor(kPressedColor);
            } else {
                button_background.setFillColor(kReleaseColor);
            }

            window->draw(button_background);

            WidgetContainer::Draw(window);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();
            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < width)
                && (mouse_pos[1] < height)) {
                return true;
            }

            return false;
        };
};

class PanelControl : public WidgetContainer {
    public:
        explicit PanelControl(const PanelControl& other)
            :WidgetContainer(other) {
            Widget::SetParent(other.GetParent());
            WidgetContainer::SetParentToChildren();
        };

        explicit PanelControl(const Coordinates& lt_corner, float width, float height,
                              const std::vector<Widget*>* buttons = NULL, Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height, buttons) {
            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            sf::RectangleShape button_background(sf::Vector2f(width, height));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            button_background.setFillColor(kPanelColor);

            window->draw(button_background);

            WidgetContainer::Draw(window);
        };
};

#endif // BUTTON_HPP
