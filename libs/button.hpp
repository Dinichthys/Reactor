#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>

#include "window.hpp"

const sf::Color kDefaultButtonColor = sf::Color::Blue;
const sf::Color kPressedColor = sf::Color::Green;
const sf::Color kReleaseColor = sf::Color::Red;
const sf::Color kPanelColor = sf::Color::White;

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

class PanelControl : public Window {
    private:
        std::vector<Button*> buttons;

    public:
        explicit PanelControl(Coordinates lt_corner, Coordinates rb_corner, std::vector<Button*>& buttons_val)
            :Window(lt_corner, rb_corner) {
            size_t length = buttons_val.size();
            for (size_t i = 0; i < length; i++) {
                buttons.push_back(buttons_val[i]);
            }
        };

        std::vector<Button*>& GetButtons() {return buttons;};

        virtual void Draw(sf::RenderWindow& window) override {
            Coordinates lt_corner(Window::GetLTCorner());
            Coordinates rb_corner(Window::GetRBCorner());

            sf::RectangleShape button_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            button_background.setFillColor(kPanelColor);

            window.draw(button_background);

            size_t buttons_num = buttons.size();
            for (size_t i = 0; i < buttons_num; i++) {
                buttons[i]->Draw(window);
            }
        };
        virtual void Move(float shift_x, float shift_y) override {
            Window::Move(shift_x, shift_y);

            size_t buttons_num = buttons.size();
            for (size_t i = 0; i < buttons_num; i++) {
                buttons[i]->Move(shift_x, shift_y);
            }
        };
};

#endif // BUTTON_HPP
