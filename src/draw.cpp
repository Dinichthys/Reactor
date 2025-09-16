#include "draw.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "graph.hpp"
#include "circle.hpp"
#include "cubes.hpp"

#include "logging.h"
#include "my_assert.h"

RendererError Renderer::ShowWindow() {
    const Reactor& reactor = scene_manager.GetReactor();
    const Coordinates& lt_corner = reactor.GetLTCorner();
    const Coordinates& rb_corner = reactor.GetRBCorner();
    sf::RectangleShape reactor_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
    reactor_background.setPosition(lt_corner[0], lt_corner[1]);
    reactor_background.setFillColor(sf::Color::Cyan);

    sf::RectangleShape background(sf::Vector2f((float)screen_width, (float)screen_height));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color::Black);

    std::vector<Circle>& circles = scene_manager.GetCircleVector();
    std::vector<Cube>& cubes = scene_manager.GetCubesVector();

    sf::Event event;
    while (window.isOpen()) {
        size_t circles_num = circles.size();
        size_t cubes_num = cubes.size();

        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        LOG(kDebug, "Drawing window");

        window.draw(background);
        window.draw(reactor_background);

        for (size_t i = 0; i < circles_num; i++) {
            Coordinates center(circles[i].GetCenterCoordinates());
            if ((center[0] < lt_corner[0]) || (center[0] > rb_corner[0])
                || (center[1] < lt_corner[1]) || (center[1] > rb_corner[1])) {
                circles.erase(circles.begin() + i);
                circles_num--;
                i--;
                continue;
            }

            DrawCircles(circles[i]);
            circles[i].SetCenterCoordinates(circles[i].GetCenterCoordinates() + circles[i].GetSpeed());
        }

        for (size_t i = 0; i < cubes_num; i++) {
            Coordinates center(cubes[i].GetCenterCoordinates());
            if ((center[0] < lt_corner[0]) || (center[0] > rb_corner[0])
                || (center[1] < lt_corner[1]) || (center[1] > rb_corner[1])) {
                cubes.erase(cubes.begin() + i);
                cubes_num--;
                i--;
                continue;
            }

            sf::RectangleShape rectangle(sf::Vector2f(kWidthCube, kWidthCube));
            rectangle.setPosition(sf::Vector2f(center[0] - kWidthCube / 2, center[1] - kWidthCube / 2));
            rectangle.setFillColor(sf::Color::Red);
            window.draw(rectangle);
            cubes[i].SetCenterCoordinates(cubes[i].GetCenterCoordinates() + cubes[i].GetSpeed());
        }

        CheckCollisions();

        CheckBorders();

        usleep(kTimeSleep);

        window.display();
    }

    return kDoneRenderer;
}

RendererError Renderer::CheckCollisions() {
    std::vector<Circle>& circles = scene_manager.GetCircleVector();
    size_t circles_num = circles.size();
    std::vector<Cube>& cubes = scene_manager.GetCubesVector();
    size_t cubes_num = cubes.size();

    LOG(kDebug, "Number of circles %lu\n"
                "Number of cubes %lu\n", circles_num, cubes_num);

    for (size_t i = 0; (circles_num > 1) && (i < circles_num - 1);) {
        bool reaction = false;
        for (size_t j = i + 1; j < circles_num; j++) {
            if ((circles[i].GetCenterCoordinates() - circles[j].GetCenterCoordinates()).SqLength()
                 < kCircleRadius * kCircleRadius) {
                Coordinates center_first(circles[i].GetCenterCoordinates());
                Coordinates speed_first(circles[i].GetSpeed());
                Coordinates center_second(circles[j].GetCenterCoordinates());
                Coordinates speed_second(circles[j].GetSpeed());

                cubes.push_back(Cube((center_first + center_second) / 2, 2 * kCircleWeight, speed_first + speed_second));

                circles.erase(circles.begin() + i);
                circles.erase(circles.begin() + j - 1);
                reaction = true;
                circles_num -= 2;
                cubes_num++;
                break;
            }
        }
        i = reaction ? i : i + 1;
    }

    for (size_t i = 0; i < cubes_num; i++) {
        Coordinates center_cube(cubes[i].GetCenterCoordinates());
        for (size_t j = 0; j < circles_num;) {
            Coordinates center_circle(circles[j].GetCenterCoordinates());
            if ((abs(center_cube[0] - center_circle[0]) < kWidthCube / 2 + kCircleRadius)
                && (abs(center_cube[1] - center_circle[1]) < kWidthCube / 2 + kCircleRadius)) {
                Coordinates speed_circle(circles[j].GetSpeed());
                Coordinates speed_cube(cubes[i].GetSpeed());
                size_t weight_cube = cubes[i].GetWeight();

                cubes[i] = Cube((center_cube + center_circle) / 2, weight_cube + kCircleWeight,
                                speed_cube * weight_cube + speed_circle * kCircleWeight);
                center_cube = cubes[i].GetCenterCoordinates();

                circles.erase(circles.begin() + j);
                circles_num--;
                continue;
            }
            j++;
        }
    }

    for (size_t i = 0; (cubes_num > 1) && (i < cubes_num - 1);) {
        Coordinates center_first(cubes[i].GetCenterCoordinates());
        bool reaction = false;
        for (size_t j = i + 1; j < cubes_num;) {
            Coordinates center_second(cubes[j].GetCenterCoordinates());
            if ((abs(center_first[0] - center_second[0]) < kWidthCube)
                && (abs(center_first[1] - center_second[1]) < kWidthCube)) {
                Coordinates speed_first(cubes[i].GetSpeed());
                size_t weight_first = cubes[i].GetWeight();
                Coordinates speed_second(cubes[j].GetSpeed());
                size_t weight_second = cubes[j].GetWeight();

                size_t new_circles_num = weight_first + weight_second;
                Coordinates center = (center_first + center_second) / 2;
                Coordinates speed = (speed_first * weight_first + speed_second * weight_second)
                                    / (weight_first + weight_second);
                float cos_angle = cos(2 * M_PI / new_circles_num);
                float sin_angle = sin(2 * M_PI / new_circles_num);
                for (size_t new_circle_index = 0; new_circle_index < new_circles_num; new_circle_index++) {
                    circles.push_back(Circle(center + (!speed) * kCircleRadius * 2, Coordinates(speed)));
                    float speed_x = speed[0];
                    float speed_y = speed[1];
                    speed.SetCoordinate(0, speed_x * cos_angle - speed_y * sin_angle);
                    speed.SetCoordinate(1, speed_x * sin_angle + speed_y * cos_angle);
                }

                cubes.erase(cubes.begin() + i);
                cubes.erase(cubes.begin() + j - 1);
                reaction = true;
                cubes_num -= 2;
                circles_num += new_circles_num;
                break;
            }
            j++;
        }
        i = reaction ? i : i + 1;
    }

    return kDoneRenderer;
}

RendererError Renderer::CheckBorders() {
    std::vector<Circle>& circles = scene_manager.GetCircleVector();
    size_t circles_num = circles.size();
    std::vector<Cube>& cubes = scene_manager.GetCubesVector();
    size_t cubes_num = cubes.size();
    const Reactor& reactor = scene_manager.GetReactor();
    const Coordinates& lt_corner = reactor.GetLTCorner();
    const Coordinates& rb_corner = reactor.GetRBCorner();

    for (size_t i = 0; i < circles_num; i++) {
        Coordinates center = circles[i].GetCenterCoordinates();
        Coordinates speed = circles[i].GetSpeed();

        if ((center[0] + speed[0] - lt_corner[0] < kCircleRadius) && (speed[0] < 0)) {
            circles[i].SetCenterCoordinates(center + Coordinates(2, 0, 2 * speed[1]));
            speed.SetCoordinate(0, -speed[0]);
            circles[i].SetSpeed(Coordinates(speed));
        }
        if ((center[1] + speed[1] - lt_corner[1] < kCircleRadius) && (speed[1] < 0)) {
            circles[i].SetCenterCoordinates(center + Coordinates(2, 2 * speed[0], 0));
            speed.SetCoordinate(1, -speed[1]);
            circles[i].SetSpeed(Coordinates(speed));
        }
        if ((rb_corner[0] - center[0] - speed[0] < kCircleRadius) && (speed[0] > 0)) {
            circles[i].SetCenterCoordinates(center + Coordinates(2, 0, 2 * speed[1]));
            speed.SetCoordinate(0, -speed[0]);
            circles[i].SetSpeed(Coordinates(speed));
        }
        if ((rb_corner[1] - center[1] - speed[1] < kCircleRadius) && (speed[1] > 0)) {
            circles[i].SetCenterCoordinates(center + Coordinates(2, 2 * speed[0], 0));
            speed.SetCoordinate(1, -speed[1]);
            circles[i].SetSpeed(Coordinates(speed));
        }
    }

    for (size_t i = 0; i < cubes_num; i++) {
        Coordinates center = cubes[i].GetCenterCoordinates();
        Coordinates speed = cubes[i].GetSpeed();

        if ((center[0] + speed[0] - lt_corner[0] < kWidthCube / 2) && (speed[0] < 0)) {
            cubes[i].SetCenterCoordinates(center + Coordinates(2, 0, 2 * speed[1]));
            speed.SetCoordinate(0, -speed[0]);
            cubes[i].SetSpeed(Coordinates(speed));
        }
        if ((center[1] + speed[1] - lt_corner[1] < kWidthCube / 2) && (speed[1] < 0)) {
            cubes[i].SetCenterCoordinates(center + Coordinates(2, 2 * speed[0], 0));
            speed.SetCoordinate(1, -speed[1]);
            cubes[i].SetSpeed(Coordinates(speed));
        }
        if ((rb_corner[0] - center[0] - speed[0] < kWidthCube / 2) && (speed[0] > 0)) {
            cubes[i].SetCenterCoordinates(center + Coordinates(2, 0, 2 * speed[1]));
            speed.SetCoordinate(0, -speed[0]);
            cubes[i].SetSpeed(Coordinates(speed));
        }
        if ((rb_corner[1] - center[1] - speed[1] < kWidthCube / 2) && (speed[1] > 0)) {
            cubes[i].SetCenterCoordinates(center + Coordinates(2, 2 * speed[0], 0));
            speed.SetCoordinate(1, -speed[1]);
            cubes[i].SetSpeed(Coordinates(speed));
        }
    }

    return kDoneRenderer;
}

RendererError Renderer::DrawCircles(const Circle& circle) {
    Coordinates center = circle.GetCenterCoordinates();
    sf::VertexArray vertices (sf::PrimitiveType::Points, (size_t) (4 * kCircleRadius * kCircleRadius));
    size_t vertex_index = 0;
    for (float i = -kCircleRadius; i < kCircleRadius; i++) {
        for (float j = -kCircleRadius; j < kCircleRadius; j++) {
            if (i * i + j * j < kCircleRadius * kCircleRadius) {
                vertices[vertex_index].position = {center[0] + j, center[1] + i};
                vertices[vertex_index].color = kColorCircle;
                vertex_index++;
            }
        }
    }

    window.draw(vertices);

    return kDoneRenderer;
}
