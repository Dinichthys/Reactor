#include "vector.hpp"

#include <math.h>

#include "logging.h"

// GetAngle returns value of angel in radians
float MyVector::GetAngle() {
    if (end.GetDimension() != 2) {
        return NAN;
    }

    float height = end[1] - start[1];
    float length = Length();

    float angle = asinf(height/length);

    LOG(kDebug, "Angle = %f", angle * 180 / M_PI);

    if (start[0] < end[0]) {
        return angle;
    } else {
        return M_PI - angle;
    }
}

void MyVector::Rotate(float angle) {
    ASSERT(!isnan(angle), "Invalid angle for rotation");

    float new_angle = GetAngle() + angle;
    float length = Length();

    end.SetCoordinate(0, length * cos(new_angle) + start[0]);
    end.SetCoordinate(1, length * sin(new_angle) + start[1]);
}
