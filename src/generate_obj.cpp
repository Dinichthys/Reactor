#include "generate_obj.hpp"

#include <stdlib.h>
#include <vector>

#include "object.hpp"

void GenerateObjects(std::vector<Object*>& objects, float width, float height, size_t number) {
    for(size_t i = 0; i < number; i++) {
        objects.push_back(new Circle(Coordinates(2, ((float)rand() * width) / RAND_MAX, ((float)rand() * height) / RAND_MAX),
                                    Coordinates(2, ((float)rand() * 10) / RAND_MAX, ((float)rand() * 10) / RAND_MAX)));
    }
}
