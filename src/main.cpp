#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "draw.hpp"
#include "object.hpp"

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

    std::vector<Object*> objects = {};
    for (size_t i = 0; i < circles.size(); i++) {
        try {
            objects.push_back(new Circle(circles[i]));
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            for (size_t j = 0; j < i; j++) {
                delete dynamic_cast<Circle*> (objects.back());
                objects.pop_back();
            }
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < cubes.size(); i++) {
        try {
            objects.push_back(new Cube(cubes[i]));
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            for (size_t j = 0; j < i; j++) {
                delete dynamic_cast<Cube*> (objects.back());
                objects.pop_back();
            }
            for (size_t j = 0; j < circles.size(); j++) {
                delete dynamic_cast<Circle*> (objects.back());
                objects.pop_back();
            }
            return EXIT_FAILURE;
        }
    }

    Reactor reactor(Coordinates(2, 100, 100), Coordinates(2, 980, 620));

    Renderer renderer(1080, 720, objects, reactor);

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
