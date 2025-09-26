#include "generate_obj.hpp"

#include <stdlib.h>
#include <vector>

#include "object.hpp"

GeneratingError GenerateCircleObjects(std::vector<Object*>& objects, float width, float height, size_t number) {
    for(size_t i = 0; i < number; i++) {
        objects.push_back(new(std::nothrow) Circle(Coordinates(2, ((float)rand() * width) / RAND_MAX, ((float)rand() * height) / RAND_MAX),
                                                   Coordinates(2, ((float)rand() * 10) / RAND_MAX, ((float)rand() * 10) / RAND_MAX)));

        if (objects.back() == NULL) {
            return kBadAllocGenerating;
        }
    }
    return kDoneGenerating;
}

GeneratingError GenerateCubeObjects(std::vector<Object*>& objects, float width, float height, size_t number) {
    for(size_t i = 0; i < number; i++) {
        objects.push_back(new(std::nothrow) Cube(Coordinates(2, ((float)rand() * width) / RAND_MAX, ((float)rand() * height) / RAND_MAX),
                                                 kCircleWeight,
                                                 Coordinates(2, ((float)rand() * 10) / RAND_MAX, ((float)rand() * 10) / RAND_MAX)));

        if (objects.back() == NULL) {
            return kBadAllocGenerating;
        }
    }
    return kDoneGenerating;
}
