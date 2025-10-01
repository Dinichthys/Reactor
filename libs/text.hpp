#ifndef TEXT_HPP
#define TEXT_HPP

#include "window.hpp"

class Text : public Widget {
    private:
        const char* const text_;
        sf::Font font_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height, Widget* parent,
                        const char* text_val, const char* file_name)
            :Widget(lt_corner, width, height, parent), text_(text_val) {
            ASSERT(text_val != NULL, "");
            ASSERT(file_name != NULL, "");

            font_.loadFromFile(file_name);
        };

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());
            float height = Widget::GetHeight();

            sf::Text text(text_, font_, height);
            text.setPosition(lt_corner[0], lt_corner[1]);

            window->draw(text);
        };
};

#endif // TEXT_HPP
