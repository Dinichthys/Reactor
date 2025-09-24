#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "draw.hpp"
#include "object.hpp"

#include "logging.h"

void GenerateObjects(std::vector<Object*>& objects, float width, float height);

int main() {
    set_log_lvl(kError);

    const float kWidthReactor = 500;
    const float kHeightReactor = 400;

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
    GenerateObjects(objects, kWidthReactor, kHeightReactor);
    for (size_t i = 0; i < circles.size(); i++) {
        try {
            objects.push_back(new Circle(circles[i]));
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            for (size_t j = 0; j < i; j++) {
                delete objects.back();
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
            for (size_t j = 0; j < i + circles.size(); j++) {
                delete objects.back();
                objects.pop_back();
            }
            return EXIT_FAILURE;
        }
    }

    ReactorManager reactor(Coordinates(2, 100, 100), Coordinates(2, 100 + kWidthReactor, 100 + kHeightReactor), objects);
    GraphManager graph(Coordinates(2, 500, 300), Coordinates(2, 700, 500));

    Renderer renderer(1080, 720, reactor, graph,
                      PistonButton(Button(Coordinates(2, 200, 500), Coordinates(2, 300, 600), "+"), 50),
                      PistonButton(Button(Coordinates(2, 500, 500), Coordinates(2, 600, 600), "-"), -50));

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void GenerateObjects(std::vector<Object*>& objects, float width, float height) {
    for(size_t i = 0; i < 1000; i++) {
        objects.push_back(new Circle(Coordinates(2, ((float)rand() * width) / RAND_MAX, ((float)rand() * height) / RAND_MAX),
                                    Coordinates(2, ((float)rand() * 10) / RAND_MAX, ((float)rand() * 10) / RAND_MAX)));
    }
}
