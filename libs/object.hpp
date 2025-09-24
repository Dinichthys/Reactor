#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <typeinfo>

#include "vector.hpp"

const float kWidthCube = 5;

const float kCircleRadius = 5;
const size_t kCircleWeight = 1;

class Object {
    private:
        size_t id_hash;
        Coordinates center;
        Coordinates speed;

    public:
        explicit Object(Coordinates center_val, Coordinates speed_val, const size_t id_hash_val)
            : center(center_val), speed(speed_val) {
            id_hash = id_hash_val;
        };

        virtual ~Object() {};

        virtual Coordinates GetCenterCoordinates() const {return Coordinates(center);};
        virtual Coordinates GetSpeed() const {return Coordinates(speed);};
        virtual size_t GetIDHash() const {return id_hash;};
        virtual void SetCenterCoordinates(Coordinates center) {Object::center = center;};
        virtual void SetSpeed(Coordinates speed) {Object::speed = speed;};
        virtual void SetIDHash(size_t hash_code) {id_hash = hash_code;};
};

class Cube : virtual public Object {
    private:
        size_t weight;

    public:
        explicit Cube(const Coordinates& center, size_t weight_val, Coordinates speed_val)
            :Object(Coordinates(center), Coordinates(speed_val), typeid(Cube).hash_code()) {
            weight = weight_val;
        };

        virtual ~Cube() {};

        size_t GetWeight() const {return weight;};
        void   SetWeight(size_t new_weight) {weight = new_weight;};
};

class Circle : virtual public Object {
    public:
        explicit Circle(Coordinates center = Coordinates(0, 0, 0, 0),
                        Coordinates speed = Coordinates(0, 0, 0, 0))
            :Object(Coordinates(center), Coordinates(speed), typeid(Circle).hash_code()) {};

        virtual ~Circle() {};
};

const size_t kCircleIDHashCode = (typeid(Circle)).hash_code();
const size_t kCubeIDHashCode = (typeid(Cube)).hash_code();

#endif // OBJECT_HPP
