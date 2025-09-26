#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>

#include "reactor.hpp"
#include "window.hpp"
#include "my_assert.h"

static const sf::Color kDefaultButtonColor = sf::Color::Blue;
static const sf::Color kPressedColor = sf::Color::Green;
static const sf::Color kReleaseColor = sf::Color::Red;
static const sf::Color kPanelColor = sf::Color::White;

class Button : public Window {
    private:
        bool pressed;
        const char* const text;
        sf::Font font;

    public:
        explicit Button(const Coordinates& lt_corner, const Coordinates& rb_corner,
                        const char* text_val, const char* file_name)
            :Window(lt_corner, rb_corner), text(text_val) {
            ASSERT(text_val != NULL, "");
            ASSERT(file_name != NULL, "");

            pressed = false;
            font.loadFromFile(file_name);
        };

        bool GetPressedInfo() const {return pressed;};
        const char* GetText() const {return text;};
        void SetPressedInfo(bool new_pressed) {pressed = new_pressed;};

        virtual void Draw(sf::RenderWindow* window) {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Window::GetLTCorner());
            Coordinates rb_corner(Window::GetRBCorner());

            sf::RectangleShape button_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            if (Button::GetPressedInfo()) {
                button_background.setFillColor(kPressedColor);
            } else {
                button_background.setFillColor(kReleaseColor);
            }

            sf::Text text(Button::GetText(), font, rb_corner[1] - lt_corner[1]);
            text.setPosition(lt_corner[0], lt_corner[1]);

            window->draw(button_background);
            window->draw(text);
        };

        virtual void Action(ReactorManager* reactor) {};
};

class PanelControl : public Window {
    private:
        std::vector<Button*> buttons;

    public:
        explicit PanelControl(Coordinates lt_corner, Coordinates rb_corner, const std::vector<Button*>& buttons_val)
            :Window(lt_corner, rb_corner) {
            size_t length = buttons_val.size();
            for (size_t i = 0; i < length; i++) {
                buttons.push_back(buttons_val[i]);
            }
        };

        std::vector<Button*>& GetButtons() {return buttons;};

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Window::GetLTCorner());
            Coordinates rb_corner(Window::GetRBCorner());

            sf::RectangleShape button_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            button_background.setFillColor(kPanelColor);

            window->draw(button_background);

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
