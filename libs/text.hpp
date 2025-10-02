#ifndef TEXT_HPP
#define TEXT_HPP

#include "graphics.hpp"

#include "window.hpp"

class Text : public Widget {
    private:
        const char* const text_;
        const char* const font_file_name_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height, Widget* parent,
                        const char* text_val, const char* file_name)
            :Widget(lt_corner, width, height, parent), text_(text_val), font_file_name_(file_name) {
            ASSERT(text_val != NULL, "");
            ASSERT(file_name != NULL, "");
        };

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());
            float height = Widget::GetHeight();

            graphics::Text text(text_, font_file_name_, height);
            text.SetPosition(lt_corner);

            window->Draw(text);
        };
};

#endif // TEXT_HPP
