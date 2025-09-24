#include "reactor.hpp"

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

static ReactorError CircleCircleCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);
static ReactorError CircleCubeCollision(std::vector<Object*>& objects, size_t circle_index, size_t cube_index, bool& reaction);
static ReactorError CubeCubeCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);

static enum ObjectType CheckObjectType(size_t type_hash_code) {
    if (type_hash_code == kCircleIDHashCode) {
        return kCircleType;
    }
    if (type_hash_code == kCubeIDHashCode) {
        return kCubeType;
    }
    return kNone;
}

#include "draw.hpp"

ReactorError ReactorManager::CheckCollisions() {
    size_t objects_num = objects.size();

    typedef ReactorError (*CheckCollision_t)(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);
    static const CheckCollision_t kCheckCollisionArr[2][2] = {
    //   Index        |        Circle          |        Cube        |
        [kCircleType] = {CircleCircleCollision,  CircleCubeCollision},
        [kCubeType]   = {CircleCubeCollision,    CubeCubeCollision  },
    };

    bool reaction = false;
    for (size_t i = 0; i < objects_num - 1; i++) {
        size_t first_hash_code = objects[i]->GetIDHash();
        enum ObjectType first_type = CheckObjectType(first_hash_code);

        for (size_t j = i + 1; j < objects_num; j++) {
            size_t second_hash_code = objects[j]->GetIDHash();
            enum ObjectType second_type = CheckObjectType(second_hash_code);

            enum ReactorError result = kCheckCollisionArr[first_type][second_type](objects, i, j, reaction);
            if (result != kDoneReactor) {
                return result;
            }
            if (reaction) {
                objects_num = objects.size();
                break;
            }
        }
    }

    return kDoneReactor;
}

static ReactorError CircleCircleCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction) {
    Circle* first = dynamic_cast<Circle*>(objects[i]);
    Circle* second = dynamic_cast<Circle*>(objects[j]);

    if ((first->GetCenterCoordinates() - second->GetCenterCoordinates()).SqLength()
            < kCircleRadius * kCircleRadius) {
        Coordinates center_first(first->GetCenterCoordinates());
        Coordinates speed_first(first->GetSpeed());
        Coordinates center_second(second->GetCenterCoordinates());
        Coordinates speed_second(second->GetSpeed());

        float coeff = sqrt((speed_first.SqLength() + speed_second.SqLength()) / 2);

        Object* res_object = NULL;
        try {
            res_object = new Cube((center_first + center_second) / 2, 2 * kCircleWeight,
                                  (!(speed_first + speed_second)) * coeff);
        } catch (const std::bad_alloc& e) {
            return ReactorError::kBadAllocReaction;
        }
        objects[i] = res_object;
        delete first;
        delete second;
        objects.erase(objects.begin() + j);
        reaction = true;
        return kDoneReactor;
    }
    reaction = false;
    return kDoneReactor;
}

static ReactorError CircleCubeCollision(std::vector<Object*>& objects, size_t circle_index, size_t cube_index, bool& reaction) {
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

        float coeff = sqrt((speed_circle.SqLength() * kCircleWeight + speed_cube.SqLength() * weight_cube)
                            / (weight_cube + kCircleWeight));

        cube->SetCenterCoordinates((center_cube * weight_cube + center_circle * kCircleWeight) / (weight_cube + kCircleWeight));
        cube->SetSpeed((!(speed_cube * weight_cube + speed_circle * kCircleWeight)) * coeff);
        cube->SetWeight(weight_cube + kCircleWeight);

        delete circle;
        objects.erase(objects.begin() + circle_index);
        reaction = true;
        return kDoneReactor;
    }
    reaction = false;
    return kDoneReactor;
}

static ReactorError CubeCubeCollision(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction) {
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

        float coeff = sqrt((speed_first.SqLength() * weight_first + speed_second.SqLength() * weight_second)
                            / (weight_first + weight_second));

        Coordinates speed = (!(speed_first * weight_first + speed_second * weight_second)) * coeff;
        float cos_angle = cos(2 * M_PI / new_circles_num);
        float sin_angle = sin(2 * M_PI / new_circles_num);
        for (size_t new_circle_index = 0; new_circle_index < new_circles_num; new_circle_index++) {
            Object* new_circle = NULL;
            try {
                new_circle = new Circle(center + (!speed) * kCircleRadius * 2, Coordinates(speed));
            } catch (const std::bad_alloc& e) {
                for (size_t del_index = 0; del_index < new_circle_index; del_index++) {
                    delete objects.back();
                    objects.pop_back();
                }
                return ReactorError::kBadAllocReaction;
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
        return kDoneReactor;
    }
    reaction = false;
    return kDoneReactor;
}

ReactorError ReactorManager::DrawMolecules(sf::RenderWindow& window) {
    size_t objects_num = objects.size();

    const Coordinates& lt_corner = Window::GetLTCorner();
    const Coordinates& rb_corner = Window::GetRBCorner();
    float width = piston - lt_corner[0];
    float height = rb_corner[1] - lt_corner[1];

    for (size_t i = 0; i < objects_num; i++) {
        const Object* object = objects[i];
        Coordinates center(object->GetCenterCoordinates());
        if ((center[0] < 0) || (center[0] > width)
            || (center[1] < 0) || (center[1] > height)) {
            delete objects[i];
            objects.erase(objects.begin() + i);
            objects_num--;
            i--;
            continue;
        }

        size_t object_hash_code = object->GetIDHash();
        if (object_hash_code == kCircleIDHashCode) {
            DrawCircle(window, objects[i]);
            continue;
        }
        if (object_hash_code == kCubeIDHashCode) {
            DrawCube(window, objects[i]);
            continue;
        }
        objects.erase(objects.begin() + i);
        objects_num--;
        i--;
    }

    return kDoneReactor;
}

ReactorError ReactorManager::DrawCube(sf::RenderWindow& window, const Object* const object) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinates();
    const Coordinates& lt_corner = Window::GetLTCorner();

    sf::RectangleShape rectangle(sf::Vector2f(kWidthCube, kWidthCube));
    rectangle.setPosition(sf::Vector2f(center[0] - kWidthCube / 2 + lt_corner[0],
                                       center[1] - kWidthCube / 2 + lt_corner[1]));
    rectangle.setFillColor(sf::Color::Red);
    window.draw(rectangle);

    return kDoneReactor;
}

ReactorError ReactorManager::DrawCircle(sf::RenderWindow& window, const Object* const object) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinates();
    const Coordinates& lt_corner = Window::GetLTCorner();
    sf::VertexArray vertices (sf::PrimitiveType::Points, (size_t) (4 * kCircleRadius * kCircleRadius));
    size_t vertex_index = 0;
    for (float i = -kCircleRadius; i < kCircleRadius; i++) {
        for (float j = -kCircleRadius; j < kCircleRadius; j++) {
            if (i * i + j * j < kCircleRadius * kCircleRadius) {
                vertices[vertex_index].position = {center[0] + j + lt_corner[0], center[1] + i + lt_corner[1]};
                vertices[vertex_index].color = kColorCircle;
                vertex_index++;
            }
        }
    }

    window.draw(vertices);

    return kDoneReactor;
}

ReactorError ReactorManager::MoveMolecules() {
    size_t objects_num = objects.size();

    for (size_t i = 0; i < objects_num; i++) {
        size_t object_hash_code = objects[i]->GetIDHash();
        if (object_hash_code == kCircleIDHashCode) {
            MoveCircle(objects[i], kCircleRadius);
        }
        if (object_hash_code == kCubeIDHashCode) {
            MoveCircle(objects[i], kWidthCube / 2);
        }
    }

    return kDoneReactor;
}

ReactorError ReactorManager::MoveCircle(Object* const object, float distance) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinates();
    Coordinates speed = object->GetSpeed();

    const Coordinates& lt_corner = Window::GetLTCorner();
    const Coordinates& rb_corner = Window::GetRBCorner();
    float width = piston - lt_corner[0];
    float height = rb_corner[1] - lt_corner[1];

    center = center + speed;
    if (center[0] < distance) {
        center.SetCoordinate(0, - center[0] + 2 * distance);
        speed.SetCoordinate(0, abs(speed[0]));
    }
    if (center[1] < distance) {
        center.SetCoordinate(1, - center[1] + 2 * distance);
        speed.SetCoordinate(1, abs(speed[1]));
    }
    if (width - center[0] < distance) {
        center.SetCoordinate(0, 2 * width - center[0] - 2 * distance);
        speed.SetCoordinate(0, -abs(speed[0]));
    }
    if (height - center[1] < distance) {
        center.SetCoordinate(1, 2 * height - center[1] - 2 * distance);
        speed.SetCoordinate(1, -abs(speed[1]));
    }

    object->SetCenterCoordinates(Coordinates(center));
    object->SetSpeed(Coordinates(speed));

    return kDoneReactor;
}

float ReactorManager::CountEnergy() {
    size_t objects_num = objects.size();

    float energy = 0;
    for (size_t i = 0; i < objects_num; i++) {
        float speed_sq = objects[i]->GetSpeed().SqLength();
        enum ObjectType type = CheckObjectType(objects[i]->GetIDHash());
        float weight = (type == kCircleType) ? kCircleWeight : (dynamic_cast<Cube*>(objects[i]))->GetWeight();

        energy += weight * speed_sq / 2;
    }

    return energy;
}
