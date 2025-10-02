#include "reactor.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "graphics.hpp"

#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

static ReactorError CircleCircleCollision(std::vector<Widget*>& objects, size_t i, size_t j, bool* reaction, Reactor* reactor);
static ReactorError CircleCubeCollision(std::vector<Widget*>& objects, size_t circle_index, size_t cube_index, bool* reaction, Reactor* reactor);
static ReactorError CubeCubeCollision(std::vector<Widget*>& objects, size_t i, size_t j, bool* reaction, Reactor* reactor);

#include "draw.hpp"

ReactorError Reactor::CheckCollisions() {
    std::vector<Widget*>& objects = WidgetContainer::GetChildren();
    size_t objects_num = objects.size();

    typedef ReactorError (*CheckCollision_t)(std::vector<Widget*>& objects, size_t i, size_t j, bool* reaction, Reactor* reactor);
    static const CheckCollision_t kCheckCollisionArr[kObjectsNum][kObjectsNum] = {
    //   Index        |        Circle          |        Cube        |
        [kCircleType] = {CircleCircleCollision,  CircleCubeCollision},
        [kCubeType]   = {CircleCubeCollision,    CubeCubeCollision  },
    };

    bool reaction = false;
    for (size_t i = 0; i < objects_num - 1; i++) {
        Object* object_1 = dynamic_cast<Object*>(objects[i]);
        enum ObjectType first_type = object_1->GetType();

        for (size_t j = i + 1; j < objects_num; j++) {
            Object* object_2 = dynamic_cast<Object*>(objects[j]);
            enum ObjectType second_type = object_2->GetType();

            enum ReactorError result = kCheckCollisionArr[first_type][second_type](objects, i, j, &reaction, this);
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

static ReactorError CircleCircleCollision(std::vector<Widget*>& objects, size_t i, size_t j, bool* reaction, Reactor* reactor) {
    ASSERT(reaction != NULL, "");

    Circle* first = dynamic_cast<Circle*>(objects[i]);
    if (first == NULL) {
        return kBadCast;
    }
    Circle* second = dynamic_cast<Circle*>(objects[j]);
    if (second == NULL) {
        return kBadCast;
    }

    if ((first->GetCenterCoordinatesLoc() - second->GetCenterCoordinatesLoc()).SqLength()
            < kCircleRadius * kCircleRadius) {

        Coordinates center_first(first->GetCenterCoordinatesLoc());
        Coordinates speed_first(first->GetSpeed());
        Coordinates center_second(second->GetCenterCoordinatesLoc());
        Coordinates speed_second(second->GetSpeed());

        float coeff = sqrt((speed_first.SqLength() + speed_second.SqLength()) / 2);

        Object* res_object = NULL;
        res_object = new(std::nothrow) Cube((center_first + center_second) / 2, 2 * kCircleWeight,
                                (!(speed_first + speed_second)) * coeff, reactor);
        if (res_object == NULL) {
            return ReactorError::kBadAllocReaction;
        }
        objects[i] = res_object;

        delete first;
        delete second;
        objects.erase(objects.begin() + j);
        *reaction = true;
        return kDoneReactor;
    }
    *reaction = false;
    return kDoneReactor;
}

static ReactorError CircleCubeCollision(std::vector<Widget*>& objects, size_t circle_index, size_t cube_index, bool* reaction,
                                        __attribute_maybe_unused__ Reactor* reactor) {
    ASSERT(reaction != NULL, "");

    if (dynamic_cast<Object*>(objects[circle_index])->GetType() != kCircleType) {
        size_t tmp = circle_index;
        circle_index = cube_index;
        cube_index = tmp;
    }

    Circle* circle = dynamic_cast<Circle*>(objects[circle_index]);
    if (circle == NULL) {
        return kBadCast;
    }
    Cube* cube = dynamic_cast<Cube*>(objects[cube_index]);
    if (cube == NULL) {
        return kBadCast;
    }

    Coordinates center_circle(circle->GetCenterCoordinatesLoc());
    Coordinates center_cube(cube->GetCenterCoordinatesLoc());
    if ((abs(center_cube[0] - center_circle[0]) < kWidthCube / 2 + kCircleRadius)
        && (abs(center_cube[1] - center_circle[1]) < kWidthCube / 2 + kCircleRadius)) {

        Coordinates speed_circle(circle->GetSpeed());
        Coordinates speed_cube(cube->GetSpeed());
        size_t weight_cube = cube->GetWeight();

        float coeff = sqrt((speed_circle.SqLength() * kCircleWeight + speed_cube.SqLength() * weight_cube)
                            / (weight_cube + kCircleWeight));

        Coordinates new_center = (center_cube * weight_cube + center_circle * kCircleWeight) / (weight_cube + kCircleWeight);
        cube->Move((new_center - center_cube)[0], (new_center - center_cube)[1]);
        cube->SetSpeed((!(speed_cube * weight_cube + speed_circle * kCircleWeight)) * coeff);
        cube->SetWeight(weight_cube + kCircleWeight);

        delete circle;
        objects.erase(objects.begin() + circle_index);
        *reaction = true;
        return kDoneReactor;
    }
    *reaction = false;
    return kDoneReactor;
}

static ReactorError CubeCubeCollision(std::vector<Widget*>& objects, size_t i, size_t j, bool* reaction, Reactor* reactor) {
    ASSERT(reaction != NULL, "");

    Cube* first = dynamic_cast<Cube*>(objects[i]);
    if (first == NULL) {
        return kBadCast;
    }
    Cube* second = dynamic_cast<Cube*>(objects[j]);
    if (second == NULL) {
        return kBadCast;
    }

    Coordinates center_first(first->GetCenterCoordinatesLoc());
    Coordinates center_second(second->GetCenterCoordinatesLoc());

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
            Object* new_circle = new(std::nothrow) Circle(center + (!speed) * kCircleRadius * 2, Coordinates(speed), reactor);
            if (new_circle == NULL) {
                for (size_t del_index = 0; del_index < new_circle_index; del_index++) {
                    delete objects.back();
                    objects.pop_back();
                }
                *reaction = false;
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
        *reaction = true;
        return kDoneReactor;
    }
    *reaction = false;
    return kDoneReactor;
}

ReactorError Reactor::DrawMolecules(graphics::RenderWindow* window) {
    ASSERT(window != NULL, "");

    std::vector<Widget*>& objects = WidgetContainer::GetChildren();
    size_t objects_num = objects.size();

    float width = piston_;
    float height = Widget::GetHeight();

    for (size_t i = 0; i < objects_num; i++) {
        const Object* object = dynamic_cast<Object*>(objects[i]);
        Coordinates center = object->GetCenterCoordinatesLoc();
        if ((center[0] < 0) || (center[0] > width)
            || (center[1] < 0) || (center[1] > height)) {
            delete objects[i];
            objects.erase(objects.begin() + i);
            objects_num--;
            i--;
            continue;
        }

        objects[i]->Draw(window);
    }

    return kDoneReactor;
}

ReactorError Reactor::MoveMolecules() {
    std::vector<Widget*>& objects = WidgetContainer::GetChildren();
    size_t objects_num = objects.size();

    for (size_t i = 0; i < objects_num; i++) {
        Object* object = dynamic_cast<Object*>(objects[i]);
        float radius = (object->GetType() == kCircleType) ? kCircleRadius
                                                          : kWidthCube / 2;
        MoveCircle(object, radius);
    }

    return kDoneReactor;
}

ReactorError Reactor::MoveCircle(Object* const object, float distance) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinatesLoc();
    Coordinates speed = object->GetSpeed();

    float width = piston_;
    float height = Widget::GetHeight();

    Coordinates new_center = center + speed;
    if (new_center[0] < distance) {
        new_center.SetCoordinate(0, - new_center[0] + 2 * distance);
        speed.SetCoordinate(0, abs(speed[0]));
    }
    if (new_center[1] < distance) {
        new_center.SetCoordinate(1, - new_center[1] + 2 * distance);
        speed.SetCoordinate(1, abs(speed[1]));
    }
    if (width - new_center[0] < distance) {
        new_center.SetCoordinate(0, 2 * width - new_center[0] - 2 * distance);
        speed.SetCoordinate(0, -abs(speed[0]));
    }
    if (height - new_center[1] < distance) {
        new_center.SetCoordinate(1, 2 * height - new_center[1] - 2 * distance);
        speed.SetCoordinate(1, -abs(speed[1]));
    }

    object->Move((new_center - center)[0], (new_center - center)[1]);
    object->SetSpeed(Coordinates(speed));

    return kDoneReactor;
}

float Reactor::CountEnergy() {
    std::vector<Widget*>& objects = WidgetContainer::GetChildren();
    size_t objects_num = objects.size();

    float energy = 0;
    for (size_t i = 0; i < objects_num; i++) {
        Object* object = dynamic_cast<Object*>(objects[i]);
        float speed_sq = object->GetSpeed().SqLength();
        enum ObjectType type = object->GetType();
        float weight = (type == kCircleType) ? kCircleWeight : (dynamic_cast<Cube*>(objects[i]))->GetWeight();

        energy += weight * speed_sq / 2;
    }

    return energy;
}

