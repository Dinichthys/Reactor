#ifndef GENERATE_OBJ
#define GENERATE_OBJ

#include <stdlib.h>
#include <vector>

#include "object.hpp"

enum GeneratingError {
    kDoneGenerating = 0,
    kBadAllocGenerating,
};

GeneratingError GenerateCircleObjects(std::vector<Widget*>& objects, float width, float height, size_t number,
                                      Widget* reactor);
GeneratingError GenerateCubeObjects(std::vector<Widget*>& objects, float width, float height, size_t number,
                                    Widget* reactor);

#endif // GENERATE_OBJ
