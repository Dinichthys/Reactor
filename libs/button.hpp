#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>

#include "window.hpp"

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

        // virtual void Draw(sf::RenderWindow& window) override {};
};

#endif // BUTTON_HPP
