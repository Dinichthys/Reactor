#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "object.hpp"
#include "generate_obj.hpp"
#include "draw.hpp"

#include "logging.h"

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
    GenerateObjects(objects, kWidthReactor, kHeightReactor, 1000);
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

    PistonButton plus_button(Button(Coordinates(2, 250, 550), Coordinates(2, 300, 600), "->"), 50);
    PistonButton minus_button(Button(Coordinates(2, 400, 550), Coordinates(2, 450, 600), "<-"), -50);

    std::vector<Button*> buttons;
    buttons.push_back(&plus_button);
    buttons.push_back(&minus_button);
    PanelControl panel_control(Coordinates(2, 200, 500), Coordinates(2, 500, 650), buttons);

    Renderer renderer(1080, 720, reactor, graph, panel_control);

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
