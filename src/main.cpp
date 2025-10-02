#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "object.hpp"
#include "generate_obj.hpp"
#include "draw.hpp"
#include "clock.hpp"
#include "reactor_button.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    std::vector<Circle> circles = {
        // Circle(Coordinates(2, 200, 200), Coordinates(2, 5, 0)),
        // Circle(Coordinates(2, 700, 200), Coordinates(2, 5, 0)),
        // Circle(Coordinates(2, 500, 200), Coordinates(2, 5, 0)),
        // Circle(Coordinates(2, 500, 300), Coordinates(2, 5, 0)),
        // Circle(Coordinates(2, 900, 600), Coordinates(2, -5, 3)),
        // Circle(Coordinates(2, 200, 400), Coordinates(2, -5, 3)),
        // Circle(Coordinates(2, 400, 200), Coordinates(2, -5, 3)),
        // Circle(Coordinates(2, 500, 300), Coordinates(2, -5, 3)),
    };

    std::vector<Cube> cubes = {
        // Cube(Coordinates(2, 900, 600), 1, Coordinates(2, -5, 3)),
        // Cube(Coordinates(2, 200, 400), 1, Coordinates(2, -5, 3)),
        // Cube(Coordinates(2, 400, 200), 1, Coordinates(2, -5, 3)),
        // Cube(Coordinates(2, 500, 300), 1, Coordinates(2, -5, 3)),
    };

    std::vector<Widget*> objects = {};
    GenerateCircleObjects(objects, kWidthReactor, kHeightReactor, 1000, NULL);
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

    GraphManager graph(Coordinates(2, 600, 300), kWidthGraph / 2, kHeightGraph / 2);

    float button_width = 205;
    float button_height = 50;

    PistonButton plus_button(Button(Coordinates(2, 40, 50), button_width, button_height, "Piston->", kFontFileName), 50);

    PistonButton minus_button(Button(Coordinates(2, 265, 50), button_width, button_height, "Piston<-", kFontFileName), -50);

    button_width = 190;

    NumberMoleculeButton plus_circle(Button(Coordinates(2, 280, 150), button_width, button_height,
                                     "Circle +", kFontFileName), 100, kCircleType);
    NumberMoleculeButton minus_circle(Button(Coordinates(2, 40, 150), button_width, button_height,
                                     "Circle -", kFontFileName), -100, kCircleType);
    NumberMoleculeButton plus_cube(Button(Coordinates(2, 280, 250), button_width, button_height,
                                     "Cube +", kFontFileName), 100, kCubeType);
    NumberMoleculeButton minus_cube(Button(Coordinates(2, 40, 250), button_width, button_height,
                                     "Cube -", kFontFileName), -100, kCubeType);

    std::vector<Widget*> buttons;
    buttons.push_back(new(std::nothrow) PistonButton(plus_button));
    buttons.push_back(new(std::nothrow) PistonButton(minus_button));
    buttons.push_back(new(std::nothrow) NumberMoleculeButton(plus_circle));
    buttons.push_back(new(std::nothrow) NumberMoleculeButton(minus_circle));
    buttons.push_back(new(std::nothrow) NumberMoleculeButton(plus_cube));
    buttons.push_back(new(std::nothrow) NumberMoleculeButton(minus_cube));
    PanelControl panel_control(Coordinates(2, 600, -50), 500, 350, &buttons);

    std::vector<Widget*> desktop_children;
    desktop_children.push_back(new(std::nothrow) ReactorManager(Coordinates(2, 100, 100), kWidthReactor, kHeightReactor, objects, graph, panel_control));
    panel_control.ResetChildren();

    desktop_children.push_back(new(std::nothrow) Clock(Coordinates(2, 100, 400), 100, 100));

    UI renderer(kStartWidth, kStartHeight, desktop_children);

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
