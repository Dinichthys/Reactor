#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>

#include "window.hpp"

const sf::Color kDefaultButtonColor = sf::Color::Blue;
const sf::Color kPressedColor = sf::Color::Green;
const sf::Color kReleaseColor = sf::Color::Red;

class Button : public Window {
    private:
        bool pressed;
        const char* const text;

    public:
        explicit Button(const Coordinates& lt_corner, const Coordinates& rb_corner, const char* text_val)
            :Window(lt_corner, rb_corner), text(text_val) {
            pressed = false;
        };

        bool GetPressedInfo() const {return pressed;};
        const char* GetText() const {return text;};
        void SetPressedInfo(bool new_pressed) {pressed = new_pressed;};

        virtual void Draw(sf::RenderWindow& window) override {
            Coordinates lt_corner(Window::GetLTCorner());
            Coordinates rb_corner(Window::GetRBCorner());

            sf::RectangleShape button_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            button_background.setFillColor(kDefaultButtonColor);

            window.draw(button_background);
        };
};

#endif // BUTTON_HPP
