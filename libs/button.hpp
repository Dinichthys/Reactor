#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <stdexcept>

#include "graphics.hpp"

#include "window.hpp"
#include "my_assert.h"
#include "text.hpp"

static const graphics::Color kDefaultButtonColor = graphics::kColorBlue;
static const graphics::Color kPressedColor = graphics::kColorGreen;
static const graphics::Color kReleaseColor = graphics::kColorRed;
static const graphics::Color kPanelColor = graphics::kColorWhite;

static uint8_t kHoveredColorScale = 2;

static const float kTextInButtonShiftVer = 0.05f;
static const float kTextInButtonShiftHor = 0.1f;

class Button : public WidgetContainer {
    private:
        bool pressed_;
        graphics::RectangleShape button_background_;
        graphics::Color pressed_color_;
        graphics::Color released_color_;

    public:
        explicit Button(const Button& other)
            :WidgetContainer(other), button_background_(other.GetWidth(), other.GetHeight()) {
            pressed_ = other.GetPressedInfo();

            pressed_color_ = other.GetPressedColor();
            released_color_ = other.GetReleasedColor();

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
                        const char* text = NULL, const char* file_name = NULL, Widget* parent = NULL,
                        graphics::Color pressed_color = kPressedColor,
                        graphics::Color released_color = kReleaseColor)
            :WidgetContainer(lt_corner, width, height), button_background_(width, height),
             pressed_color_(pressed_color), released_color_(released_color) {
            pressed_ = false;

            if ((text != NULL) && (file_name != NULL)) {
                WidgetContainer::AddChild(new(std::nothrow) Text(
                                                            Coordinates(2, width * kTextInButtonShiftHor, - height * kTextInButtonShiftVer),
                                                            width, height, this, text, file_name));
                if (WidgetContainer::GetChildren().back() == NULL) {
                    throw std::runtime_error("Bad allocation for text");
                }
            }

            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        bool GetPressedInfo() const {return pressed_;};
        void SetPressedInfo(bool new_pressed) {pressed_ = new_pressed;};

        graphics::Color GetPressedColor() const {return pressed_color_;};
        graphics::Color GetReleasedColor() const {return released_color_;};

        virtual void Draw(graphics::RenderWindow* window) {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());

            button_background_.SetPosition(lt_corner);
            graphics::Color color = (Button::GetPressedInfo()) ? pressed_color_ : released_color_;

            if (Widget::GetHovered()) {
                color  = graphics::Color(color.GetRedPart() / kHoveredColorScale,
                                         color.GetBluePart() / kHoveredColorScale,
                                         color.GetGreenPart() / kHoveredColorScale,
                                         color.GetBrightness());
            }

            button_background_.SetFillColor(color);
            window->Draw(button_background_);

            WidgetContainer::Draw(window);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            *widget = NULL;

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();
            pressed_ = false;

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {

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

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            graphics::RectangleShape button_background(width, height);
            button_background.SetPosition(lt_corner);
            button_background.SetFillColor(kPanelColor);

            window->Draw(button_background);

            WidgetContainer::Draw(window);
        };
};

#endif // BUTTON_HPP
