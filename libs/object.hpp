#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vector.hpp"

const float kWidthCube = 5;

const float kCircleRadius = 5;
const size_t kCircleWeight = 1;

enum ObjectType {
    kNone = -1,

    kCircleType = 0,
    kCubeType,

    kObjectsNum,
};

class Object {
    private:
        enum ObjectType type;
        Coordinates center;
        Coordinates speed;

    public:
        explicit Object(const Coordinates& center_val, const Coordinates& speed_val, const enum ObjectType type_val)
            : center(center_val), speed(speed_val) {
            type = type_val;
        };

        virtual ~Object() {};

        virtual Coordinates GetCenterCoordinates() const {return Coordinates(center);};
        virtual Coordinates GetSpeed() const {return Coordinates(speed);};
        virtual enum ObjectType GetType() const {return type;};
        virtual void SetCenterCoordinates(Coordinates center) {Object::center = center;};
        virtual void SetSpeed(Coordinates speed) {Object::speed = speed;};
        virtual void SetType(enum ObjectType type_val) {type = type_val;};
};

class Cube : virtual public Object {
    private:
        size_t weight;

    public:
        explicit Cube(const Coordinates& center, size_t weight_val, const Coordinates& speed_val)
            :Object(center, speed_val, kCubeType) {
            weight = weight_val;
        };

        virtual ~Cube() {};

        size_t GetWeight() const {return weight;};
        void   SetWeight(size_t new_weight) {weight = new_weight;};
};

class Circle : virtual public Object {
    public:
        explicit Circle(const Coordinates& center, const Coordinates& speed)
            :Object(center, speed, kCircleType) {};

        virtual ~Circle() {};
};

#endif // OBJECT_HPP
