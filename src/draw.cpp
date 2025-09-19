#include "draw.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

enum ObjectType {
    kNone = -1,

    kCircleType = 0,
    kCubeType = 1,
};

static enum ObjectType CheckObjectType(size_t type_hash_code);
static RendererError CircleCircleCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);
static RendererError CircleCubeCollision(std::vector<Object*>& objects, size_t circle_index, size_t cube_index, bool& reaction);
static RendererError CubeCubeCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);

typedef RendererError (*CheckCollision_t)(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);
const CheckCollision_t kCheckCollisionArr[2][2] = {
//   Index        |        Circle          |        Cube        |
    [kCircleType] = {CircleCircleCollision,  CircleCubeCollision},
    [kCubeType]   = {CircleCubeCollision,    CubeCubeCollision  },
};

static enum ObjectType CheckObjectType(size_t type_hash_code) {
    if (type_hash_code == kCircleIDHashCode) {
        return kCircleType;
    }
    if (type_hash_code == kCubeIDHashCode) {
        return kCubeType;
    }
    return kNone;
}

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

    std::vector<Object*>& objects = scene_manager.GetObjectsVector();
    size_t objects_num = objects.size();

    sf::Event event;
    while (window.isOpen()) {
        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        LOG(kDebug, "Drawing window");

        window.draw(background);
        window.draw(reactor_background);

        for (size_t i = 0; i < objects_num; i++) {
            const Object* object = objects[i];
            Coordinates center(object->GetCenterCoordinates());
            if ((center[0] < lt_corner[0]) || (center[0] > rb_corner[0])
                || (center[1] < lt_corner[1]) || (center[1] > rb_corner[1])) {
                objects.erase(objects.begin() + i);
                objects_num--;
                i--;
                continue;
            }

            size_t object_hash_code = object->GetIDHash();
            if (object_hash_code == kCircleIDHashCode) {
                DrawCircle(objects[i]);
                continue;
            }
            if (object_hash_code == kCubeIDHashCode) {
                DrawCube(objects[i]);
                continue;
            }
            objects.erase(objects.begin() + i);
            objects_num--;
            i--;
        }

        bool reaction = false;
        for (size_t i = 0; i < objects_num - 1; i++) {
            size_t first_hash_code = objects[i]->GetIDHash();
            enum ObjectType first_type = CheckObjectType(first_hash_code);
            for (size_t j = i + 1; j < objects_num; j++) {
                size_t second_hash_code = objects[j]->GetIDHash();
                enum ObjectType second_type = CheckObjectType(second_hash_code);

                enum RendererError result = kCheckCollisionArr[first_type][second_type](objects, i, j, reaction);
                if (result != kDoneRenderer) {
                    return result;
                }
                if (reaction) {
                    objects_num = objects.size();
                    break;
                }
            }
        }

        for (size_t i = 0; i < objects_num; i++) {
            size_t object_hash_code = objects[i]->GetIDHash();
            if (object_hash_code == kCircleIDHashCode) {
                MoveCircle(objects[i]);
            }
            if (object_hash_code == kCubeIDHashCode) {
                MoveCube(objects[i]);
            }
        }

        usleep(kTimeSleep);

        window.display();
    }

    return kDoneRenderer;
}

static RendererError CircleCircleCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction) {
    Circle* first = dynamic_cast<Circle*>(objects[i]);
    Circle* second = dynamic_cast<Circle*>(objects[j]);

    if ((first->GetCenterCoordinates() - second->GetCenterCoordinates()).SqLength()
            < kCircleRadius * kCircleRadius) {
        Coordinates center_first(first->GetCenterCoordinates());
        Coordinates speed_first(first->GetSpeed());
        Coordinates center_second(second->GetCenterCoordinates());
        Coordinates speed_second(second->GetSpeed());

        Object* res_object = NULL;
        try {
            res_object = new Cube((center_first + center_second) / 2, 2 * kCircleWeight, speed_first + speed_second);
        } catch (const std::bad_alloc& e) {
            return kBadAllocReaction;
        }
        objects[i] = res_object;
        delete first;
        delete second;
        objects.erase(objects.begin() + j);
        reaction = true;
        return kDoneRenderer;
    }
    reaction = false;
    return kDoneRenderer;
}

static RendererError CircleCubeCollision(std::vector<Object*>& objects, size_t circle_index, size_t cube_index, bool& reaction) {
    if ((objects[circle_index])->GetIDHash() != kCircleIDHashCode) {
        size_t tmp = circle_index;
        circle_index = cube_index;
        cube_index = tmp;
    }

    Circle* circle = dynamic_cast<Circle*>(objects[circle_index]);
    Cube* cube = dynamic_cast<Cube*>(objects[cube_index]);

    Coordinates center_circle(circle->GetCenterCoordinates());
    Coordinates center_cube(cube->GetCenterCoordinates());
    if ((abs(center_cube[0] - center_circle[0]) < kWidthCube / 2 + kCircleRadius)
        && (abs(center_cube[1] - center_circle[1]) < kWidthCube / 2 + kCircleRadius)) {
        Coordinates speed_circle(circle->GetSpeed());
        Coordinates speed_cube(cube->GetSpeed());
        size_t weight_cube = cube->GetWeight();

        cube->SetCenterCoordinates((center_cube * weight_cube + center_circle * kCircleWeight) / (weight_cube + kCircleWeight));
        cube->SetSpeed((speed_cube * weight_cube + speed_circle * kCircleWeight) / (weight_cube + kCircleWeight));
        cube->SetWeight(weight_cube + kCircleWeight);

        delete circle;
        objects.erase(objects.begin() + circle_index);
        reaction = true;
        return kDoneRenderer;
    }
    reaction = false;
    return kDoneRenderer;
}

static RendererError CubeCubeCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction) {
    Cube* first = dynamic_cast<Cube*>(objects[i]);
    Cube* second = dynamic_cast<Cube*>(objects[j]);

    Coordinates center_first(first->GetCenterCoordinates());
    Coordinates center_second(second->GetCenterCoordinates());

    if ((abs(center_first[0] - center_second[0]) < kWidthCube)
        && (abs(center_first[1] - center_second[1]) < kWidthCube)) {
        Coordinates speed_first(first->GetSpeed());
        size_t weight_first = first->GetWeight();
        Coordinates speed_second(second->GetSpeed());
        size_t weight_second = second->GetWeight();

        size_t new_circles_num = weight_first + weight_second;
        Coordinates center = (center_first + center_second) / 2;
        Coordinates speed = (speed_first * weight_first + speed_second * weight_second)
                            / (weight_first + weight_second);
        float cos_angle = cos(2 * M_PI / new_circles_num);
        float sin_angle = sin(2 * M_PI / new_circles_num);
        for (size_t new_circle_index = 0; new_circle_index < new_circles_num; new_circle_index++) {
            Object* new_circle = NULL;
            try {
                new_circle = new Circle(center + (!speed) * kCircleRadius * 2, Coordinates(speed));
            } catch (const std::bad_alloc& e) {
                for (size_t del_index = 0; del_index < new_circle_index; del_index++) {
                    delete dynamic_cast<Circle*> (objects.back());
                    objects.pop_back();
                }
                return kBadAllocReaction;
            }
            objects.push_back(new_circle);

            float speed_x = speed[0];
            float speed_y = speed[1];
            speed.SetCoordinate(0, speed_x * cos_angle - speed_y * sin_angle);
            speed.SetCoordinate(1, speed_x * sin_angle + speed_y * cos_angle);
        }

        delete first;
        delete second;
        objects.erase(objects.begin() + i);
        objects.erase(objects.begin() + j - 1);
        reaction = true;
        return kDoneRenderer;
    }
    reaction = false;
    return kDoneRenderer;
}

RendererError Renderer::DrawCube(const Object* const object) {
    ASSERT(object != NULL, "");

    Cube cube(*(dynamic_cast<const Cube*>(object)));
    Coordinates center = cube.GetCenterCoordinates();

    sf::RectangleShape rectangle(sf::Vector2f(kWidthCube, kWidthCube));
    rectangle.setPosition(sf::Vector2f(center[0] - kWidthCube / 2, center[1] - kWidthCube / 2));
    rectangle.setFillColor(sf::Color::Red);
    window.draw(rectangle);

    return kDoneRenderer;
}

RendererError Renderer::DrawCircle(const Object* const object) {
    ASSERT(object != NULL, "");

    Circle circle(*(dynamic_cast<const Circle*>(object)));
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

RendererError Renderer::MoveCube(Object* const object) {
    ASSERT(object != NULL, "");

    Cube* cube = dynamic_cast<Cube*>(object);
    Coordinates center = cube->GetCenterCoordinates();
    Coordinates speed = cube->GetSpeed();

    const Reactor& reactor = scene_manager.GetReactor();
    const Coordinates& lt_corner = reactor.GetLTCorner();
    const Coordinates& rb_corner = reactor.GetRBCorner();

    float distance = kWidthCube / 2;

    center = center + speed;
    if ((center[0] - lt_corner[0] < distance) && (speed[0] < 0)) {
        center.SetCoordinate(0, 2 * lt_corner[0] - center[0] + 2 * distance);
        speed.SetCoordinate(0, -speed[0]);
    }
    if ((center[1] - lt_corner[1] < distance) && (speed[1] < 0)) {
        center.SetCoordinate(1, 2 * lt_corner[1] - center[1] + 2 * distance);
        speed.SetCoordinate(1, -speed[1]);
    }
    if ((rb_corner[0] - center[0] < distance) && (speed[0] > 0)) {
        center.SetCoordinate(0, 2 * rb_corner[0] - center[0] - 2 * distance);
        speed.SetCoordinate(0, -speed[0]);
    }
    if ((rb_corner[1] - center[1] < distance) && (speed[1] > 0)) {
        center.SetCoordinate(1, 2 * rb_corner[1] - center[1] - 2 * distance);
        speed.SetCoordinate(1, -speed[1]);
    }

    cube->SetCenterCoordinates(Coordinates(center));
    cube->SetSpeed(Coordinates(speed));

    return kDoneRenderer;
}

RendererError Renderer::MoveCircle(Object* const object) {
    ASSERT(object != NULL, "");

    Circle* circle = dynamic_cast<Circle*>(object);
    Coordinates center = circle->GetCenterCoordinates();
    Coordinates speed = circle->GetSpeed();

    const Reactor& reactor = scene_manager.GetReactor();
    const Coordinates& lt_corner = reactor.GetLTCorner();
    const Coordinates& rb_corner = reactor.GetRBCorner();

    center = center + speed;
    if ((center[0] - lt_corner[0] < kCircleRadius) && (speed[0] < 0)) {
        center.SetCoordinate(0, 2 * lt_corner[0] + 2 * kCircleRadius - center[0]);
        speed.SetCoordinate(0, -speed[0]);
    }
    if ((center[1] - lt_corner[1] < kCircleRadius) && (speed[1] < 0)) {
        center.SetCoordinate(1, 2 * lt_corner[1] + 2 * kCircleRadius - center[1]);
        speed.SetCoordinate(1, -speed[1]);
    }
    if ((rb_corner[0] - center[0] < kCircleRadius) && (speed[0] > 0)) {
        center.SetCoordinate(0, 2 * rb_corner[0] - 2 * kCircleRadius - center[0]);
        speed.SetCoordinate(0, -speed[0]);
    }
    if ((rb_corner[1] - center[1] < kCircleRadius) && (speed[1] > 0)) {
        center.SetCoordinate(1, 2 * rb_corner[1] - 2 * kCircleRadius - center[1]);
        speed.SetCoordinate(1, -speed[1]);
    }

    circle->SetCenterCoordinates(Coordinates(center));
    circle->SetSpeed(Coordinates(speed));

    return kDoneRenderer;
}

const char* ErrorHandler(enum RendererError error) {
    switch (error) {
        case kDoneRenderer:
            return "All were done";
        case kBadAllocReaction:
            return "Can't allocate memory in reaction";
        default:
            return "Invalid enum RendererError identifier";
    }
}
