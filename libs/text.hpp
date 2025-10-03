#ifndef TEXT_HPP
#define TEXT_HPP

#include "graphics.hpp"

#include "window.hpp"

class Text : public Widget {
    private:
        graphics::Text text_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height, Widget* parent,
                        const char* text, const char* font_file_name)
            :Widget(lt_corner, width, height, parent), text_(text, font_file_name, height) {};

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());

            text_.SetPosition(lt_corner);
            window->Draw(text_);
        };
};

#endif // TEXT_HPP
