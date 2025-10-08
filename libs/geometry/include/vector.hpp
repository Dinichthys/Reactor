#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <math.h>
#include <malloc.h>
#include <stdint.h>

#include "my_assert.h"

static const float kVectorWidth = 1.f;
static const float kArrowLenPercentage  = 0.125f;
static const float kArrowAngle = 30.f;
static const float kEpsilon = 0.01f;
static const float kRotationAngle = 5.f * M_PI / 180;

enum CoordinatesError {
    kDoneCoordinates    = 0,
    kCantSetCoordinates = 1,
};

class Coordinates {
    private:
        float x;
        float y;
        float z;
        size_t dimension;

    public:
        explicit Coordinates(const Coordinates& need_copy) {
            dimension = need_copy.dimension;
            x = need_copy.x;
            y = need_copy.y;
            z = need_copy.z;
        };
        explicit Coordinates(size_t dimension, float x = 0, float y = 0, float z = 0) {
            Coordinates::dimension = dimension;
            Coordinates::x = x;
            Coordinates::y = y;
            Coordinates::z = z;
        };

        CoordinatesError SetCoordinate(size_t index, float value) {
            if (index >= dimension) {
                return kCantSetCoordinates;
            }
            switch (index) {
                case 0:
                    x = value;
                    break;
                case 1:
                    y = value;
                    break;
                case 2:
                    z = value;
                    break;
            };
            return kDoneCoordinates;
        };

        float GetCoordinate(size_t index) const {
            if (index >= dimension) {
                return NAN;
            }
            switch (index) {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                default:
                    return NAN;
            };
        };

        size_t GetDimension() const {
            return dimension;
        };

        float SqLength() const {
            return (*this) && (*this);
        };

        float GetModule() const {
            return sqrt(SqLength());
        };

        float operator [] (size_t index) const {
            return GetCoordinate(index);
        };

        Coordinates operator + (const Coordinates& a) const {
            return Coordinates(dimension, x + a.x, y + a.y, z + a.z);
        };

        Coordinates operator + (float val) const {
            return Coordinates(dimension, x + val, y + val, z + val);
        };

        Coordinates operator - (const Coordinates& a) const {
            return Coordinates(dimension, x - a.x, y - a.y, z - a.z);
        };

        Coordinates operator - (float val) const {
            return Coordinates(dimension, x - val, y - val, z - val);
        };

        Coordinates operator * (float val) const {
            return Coordinates(dimension, x * val, y * val, z * val);
        };

        Coordinates operator / (float val) const {
            return Coordinates(dimension, x / val, y / val, z / val);
        };

        Coordinates operator ! () {
            float len = GetModule();
            return *this / len;
        };

        float operator && (const Coordinates& a) const {
            return x * a.x + y * a.y + z * a.z;
        };

        Coordinates operator || (const Coordinates& a) const {
            return Coordinates(dimension, y * a.z - z * a.y,
                                          z * a.x - x * a.z,
                                          x * a.y - y * a.x);
        };

        void operator = (const Coordinates& need_copy) {
            dimension = need_copy.dimension;
            x = need_copy.x;
            y = need_copy.y;
            z = need_copy.z;
        };
};

class MyVector {
    private:
        Coordinates start;
        Coordinates end;

    public:
        explicit MyVector(Coordinates start_val, Coordinates end_val)
            :start(start_val), end(end_val) {
            if (start.GetDimension() != end.GetDimension()) {
                throw "Different dimensions of the ends of vector";
            }
        };

        Coordinates GetStartCoordinates() {return Coordinates(start);};
        Coordinates GetEndCoordinates() {return Coordinates(end);};

        float GetAngle();
        float Length() {
            size_t size = start.GetDimension();
            float len_sq = 0;
            for (size_t i = 0; i < size; i++) {
                float x = start[i] - end[i];
                len_sq += x * x;
            }
            return sqrt(len_sq);
        }
        void Rotate(float angle);
};

#endif // VECTOR_HPP
