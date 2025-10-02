#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <time.h>

#include "window.hpp"

static const sf::Color kColorClock = sf::Color::Blue;
static const sf::Color kColorArrow = sf::Color::Yellow;

class Clock : public Widget {
    private:
        time_t time_;

    public:
        explicit Clock(const Coordinates& lt_corner, const float width, float height,
                       Widget* parent = NULL)
            :Widget(lt_corner, width, height, parent) {};

        virtual void Draw(sf::RenderWindow* window) override {
            const Coordinates lt_corner(Widget::GetLTCornerAbs());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            sf::RectangleShape clock_background(sf::Vector2f(width, height));
            clock_background.setPosition(lt_corner[0], lt_corner[1]);
            clock_background.setFillColor(kColorClock);

            window->draw(clock_background);
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
        void DrawArrows(sf::RenderWindow* window, float angle) {
            Coordinates vec(2, 0, Widget::GetHeight()/2);
            float length = vec.GetModule();
            vec.SetCoordinate(0, length * cos(angle * M_PI / 180));
            vec.SetCoordinate(1, length * sin(angle * M_PI / 180));

            Coordinates start_coordinates(2, Widget::GetWidth() / 2, Widget::GetHeight() / 2);
            start_coordinates = start_coordinates + Widget::GetLTCornerAbs();

            sf::RectangleShape line({length, kVectorWidth});
            line.setPosition({start_coordinates[0], start_coordinates[1]});
            line.setFillColor(sf::Color::Black);

            line.rotate(angle);

            window->draw(line);

            sf::RectangleShape arrow({length * kArrowLenPercentage, kVectorWidth});
            arrow.setPosition(start_coordinates[0] + vec[0], start_coordinates[1] + vec[1]);
            arrow.setFillColor(sf::Color::Black);
            arrow.rotate(180 + angle - kArrowAngle);
            window->draw(arrow);

            arrow.rotate(2 * kArrowAngle);
            window->draw(arrow);
        };
};

#endif // CLOCK_HPP
