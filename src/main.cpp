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
    GenerateCircleObjects(objects, kWidthReactor, kHeightReactor, 1000);
    for (size_t i = 0; i < circles.size(); i++) {
        objects.push_back(new(std::nothrow) Circle(circles[i]));

        if (objects.back() == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (size_t j = 0; j < i; j++) {
                delete objects.back();
                objects.pop_back();
            }
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < cubes.size(); i++) {
        objects.push_back(new(std::nothrow) Cube(cubes[i]));

        if (objects.back() == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (size_t j = 0; j < i; j++) {
                delete objects.back();
                objects.pop_back();
            }
            return EXIT_FAILURE;
        }
    }

    ReactorManager reactor(Coordinates(2, 100, 100), Coordinates(2, 100 + kWidthReactor, 100 + kHeightReactor), objects);
    GraphManager graph(Coordinates(2, 500, 300), Coordinates(2, 700, 500));

    PistonButton plus_button(Button(Coordinates(2, 240, 550), Coordinates(2, 445, 600), "Piston->", kFontFileName), 50);
    PistonButton minus_button(Button(Coordinates(2, 465, 550), Coordinates(2, 670, 600), "Piston<-", kFontFileName), -50);

    NumberMoleculeButton plus_circle(Button(Coordinates(2, 480, 650), Coordinates(2, 670, 700), "Circle +", kFontFileName), 100, kCircleType);
    NumberMoleculeButton minus_circle(Button(Coordinates(2, 240, 650), Coordinates(2, 430, 700), "Circle -", kFontFileName), -100, kCircleType);
    NumberMoleculeButton plus_cube(Button(Coordinates(2, 480, 750), Coordinates(2, 670, 800), "Cube +", kFontFileName), 100, kCubeType);
    NumberMoleculeButton minus_cube(Button(Coordinates(2, 240, 750), Coordinates(2, 430, 800), "Cube -", kFontFileName), -100, kCubeType);

    std::vector<Button*> buttons;
    buttons.push_back(&plus_button);
    buttons.push_back(&minus_button);
    buttons.push_back(&plus_circle);
    buttons.push_back(&minus_circle);
    buttons.push_back(&plus_cube);
    buttons.push_back(&minus_cube);
    PanelControl panel_control(Coordinates(2, 200, 500), Coordinates(2, 700, 850), buttons);

    Renderer renderer(kStartWidth, kStartHeight, reactor, graph, panel_control);

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
