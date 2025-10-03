#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <time.h>

#include "window.hpp"

static const graphics::Color kColorClock = graphics::kColorBlue;
static const graphics::Color kColorArrow = graphics::kColorYellow;

class Clock : public Widget {
    private:
        time_t time_;
        graphics::RectangleShape clock_background_;

    public:
        explicit Clock(const Coordinates& lt_corner, const float width, float height,
                       Widget* parent = NULL)
            :Widget(lt_corner, width, height, parent), clock_background_(width, height) {
            time_ = time(NULL);
            clock_background_.SetFillColor(kColorClock);
        };

        virtual void Draw(graphics::RenderWindow* window) override {
            const Coordinates lt_corner(Widget::GetLTCornerAbs());

            clock_background_.SetPosition(lt_corner);
            window->Draw(clock_background_);

            tm* loc_tm = localtime(&time_);

            DrawArrows(window, 360 * loc_tm->tm_hour / 12 - 90);
            DrawArrows(window, 360 * loc_tm->tm_min  / 60 - 90);
            DrawArrows(window, 360 * loc_tm->tm_sec  / 60 - 90);
        };

        virtual bool OnIdle() override {
            time_ = time(NULL);
            return true;
        };

    private:
        void DrawArrows(graphics::RenderWindow* window, float angle) {
            Coordinates vec(2, 0, Widget::GetHeight()/2);
            float length = vec.GetModule();
            vec.SetCoordinate(0, length * cos(angle * M_PI / 180));
            vec.SetCoordinate(1, length * sin(angle * M_PI / 180));

            Coordinates start_coordinates(2, Widget::GetWidth() / 2, Widget::GetHeight() / 2);
            start_coordinates = start_coordinates + Widget::GetLTCornerAbs();

            graphics::RectangleShape line(length, kVectorWidth);
            line.SetPosition(start_coordinates);
            line.SetFillColor(graphics::kColorBlack);

            line.Rotate(angle);

            window->Draw(line);

            graphics::RectangleShape arrow(length * kArrowLenPercentage, kVectorWidth);
            arrow.SetPosition(start_coordinates + vec);
            arrow.SetFillColor(graphics::kColorBlack);
            arrow.Rotate(180 + angle - kArrowAngle);
            window->Draw(arrow);

            arrow.Rotate(2 * kArrowAngle);
            window->Draw(arrow);
        };
};

#endif // CLOCK_HPP
