#ifndef CUBES_HPP
#define CUBES_HPP

#include "vector.hpp"

const float kWidthCube = 50;

class Cube {
    private:
        Coordinates center;

        size_t weight;

        Coordinates speed;

    public:
        explicit Cube(Coordinates center_val, size_t weight_val, Coordinates speed_val)
            :center(center_val), speed(speed_val) {
            weight = weight_val;
        };

        Coordinates GetCenterCoordinates() const {return Coordinates(center);};
        Coordinates GetSpeed() const {return Coordinates(speed);};
        size_t GetWeight() const {return weight;};
        void SetCenterCoordinates(Coordinates center) {Cube::center = center;};
        void SetSpeed(Coordinates speed) {Cube::speed = speed;};
};

#endif // CUBES_HPP
