#include <stdlib.h>
#include <stdio.h>

#include "draw.hpp"
#include "circle.hpp"
#include "cubes.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    std::vector<Circle> circles = {
        Circle(Coordinates(2, 200, 200), Coordinates(2, 5, 0)),
        Circle(Coordinates(2, 700, 200), Coordinates(2, 5, 0)),
        Circle(Coordinates(2, 500, 200), Coordinates(2, 5, 0)),
        Circle(Coordinates(2, 500, 300), Coordinates(2, 5, 0)),
        Circle(Coordinates(2, 900, 600), Coordinates(2, -5, 3)),
        Circle(Coordinates(2, 200, 400), Coordinates(2, -5, 3)),
        Circle(Coordinates(2, 400, 200), Coordinates(2, -5, 3)),
        Circle(Coordinates(2, 500, 300), Coordinates(2, -5, 3)),
    };

    std::vector<Cube> cubes = {
        // Cube(Coordinates(2, 900, 600), 1, Coordinates(2, -5, 3)),
        // Cube(Coordinates(2, 200, 400), 1, Coordinates(2, -5, 3)),
        // Cube(Coordinates(2, 400, 200), 1, Coordinates(2, -5, 3)),
        // Cube(Coordinates(2, 500, 300), 1, Coordinates(2, -5, 3)),
    };

    Reactor reactor(Coordinates(2, 100, 100), Coordinates(2, 980, 620));

    Renderer renderer(1080, 720, circles, cubes, reactor);

    renderer.ShowWindow();

    return EXIT_SUCCESS;
}
