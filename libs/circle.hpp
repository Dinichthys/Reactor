#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "vector.hpp"

const float kCircleRadius = 50;
const size_t kCircleWeight = 1;

class Circle {
    private:
        Coordinates center;

        Coordinates speed;

    public:
        explicit Circle(Coordinates center_val = Coordinates(0, 0, 0, 0),
                        Coordinates speed_val = Coordinates(0, 0, 0, 0))
            :center(center_val), speed(speed_val) {};

        Coordinates GetCenterCoordinates() const {return Coordinates(center);};
        Coordinates GetSpeed() const {return Coordinates(speed);};
        void SetCenterCoordinates(Coordinates center) {Circle::center = center;};
        void SetSpeed(Coordinates speed) {Circle::speed = speed;};
};

#endif // CIRCLE_HPP
