#ifndef GENERATE_OBJ
#define GENERATE_OBJ

#include <stdlib.h>
#include <vector>

#include "object.hpp"

enum GeneratingError {
    kDoneGenerating = 0,
    kBadAllocGenerating,
};

GeneratingError GenerateCircleObjects(std::vector<Object*>& objects, float width, float height, size_t number);
GeneratingError GenerateCubeObjects(std::vector<Object*>& objects, float width, float height, size_t number);

#endif // GENERATE_OBJ
