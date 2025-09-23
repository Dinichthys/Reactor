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
    sf::RectangleShape background(sf::Vector2f((float)screen_width, (float)screen_height));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color::Black);

    sf::Event event;
    while (window.isOpen()) {
        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            AnalyseKey(event);
        }

        LOG(kDebug, "Drawing window");

        window.draw(background);

        DrawButtons();

        DrawReactor();

        DrawCoordinatesSystem();

        usleep(kTimeSleep);

        window.display();
    }

    return kDoneRenderer;
}

RendererError Renderer::AnalyseKey(const sf::Event event) {
    static bool moving = false;
    static int mouse_x = 0;
    static int mouse_y = 0;
    static Window* moving_window = NULL;

    if (event.type == sf::Event::MouseButtonPressed) {
        if (moving) {
            moving = false;
            mouse_x = 0;
            mouse_y = 0;

            return kDoneRenderer;
        }

        moving = true;
        mouse_x = sf::Mouse::getPosition(window).x;
        mouse_y = sf::Mouse::getPosition(window).y;

        moving_window = IdentifyWindow(mouse_x, mouse_y);
        if (moving_window != NULL) {
            return kDoneRenderer;
        }
        moving = false;

        PistonButton* pressed_button = IdentifyButton(mouse_x, mouse_y);
        mouse_x = 0;
        mouse_y = 0;
        if(pressed_button == NULL) {
            return kDoneRenderer;
        }

        bool pressed = pressed_button->GetPressedInfo();
        pressed_button->SetPressedInfo(!pressed);
        if (!pressed) {
            reactor_manager.SetPistonX(reactor_manager.GetPistonX() + pressed_button->GetShift());
        }
        return kDoneRenderer;
    }

    if ((moving) && (event.type == sf::Event::MouseMoved)) {
        int old_x = mouse_x;
        int old_y = mouse_y;
        mouse_x = sf::Mouse::getPosition(window).x;
        mouse_y = sf::Mouse::getPosition(window).y;

        Coordinates lt_corner(moving_window->GetLTCorner());
        Coordinates rb_corner(moving_window->GetRBCorner());

        lt_corner.SetCoordinate(0, lt_corner[0] + (float)(mouse_x - old_x));
        lt_corner.SetCoordinate(1, lt_corner[1] + (float)(mouse_y - old_y));
        rb_corner.SetCoordinate(0, rb_corner[0] + (float)(mouse_x - old_x));
        rb_corner.SetCoordinate(1, rb_corner[1] + (float)(mouse_y - old_y));

        moving_window->SetLTCorner(lt_corner);
        moving_window->SetRBCorner(rb_corner);
    }

    return kDoneRenderer;
}

Window* Renderer::IdentifyWindow(float x, float y) {
    const Coordinates& lt_corner_reactor = reactor_manager.GetLTCorner();
    const Coordinates& rb_corner_reactor = reactor_manager.GetRBCorner();
    if ((x > lt_corner_reactor[0]) && (x < rb_corner_reactor[0])
        && (y > lt_corner_reactor[1]) && (y < rb_corner_reactor[1])) {
        return &reactor_manager;
    }

    const Coordinates& lt_corner_graph = graph_manager.GetLTCorner();
    const Coordinates& rb_corner_graph = graph_manager.GetRBCorner();
    if ((x > lt_corner_graph[0]) && (x < rb_corner_graph[0])
        && (y > lt_corner_graph[1]) && (y < rb_corner_graph[1])) {
        return &graph_manager;
    }

    return NULL;
}

PistonButton* Renderer::IdentifyButton(float x, float y) {
    const Coordinates& lt_corner_plus = plus_piston.GetLTCorner();
    const Coordinates& rb_corner_plus = plus_piston.GetRBCorner();
    if ((x > lt_corner_plus[0]) && (x < rb_corner_plus[0])
        && (y > lt_corner_plus[1]) && (y < rb_corner_plus[1])) {
        return &plus_piston;
    }

    const Coordinates& lt_corner_minus = minus_piston.GetLTCorner();
    const Coordinates& rb_corner_minus = minus_piston.GetRBCorner();
    if ((x > lt_corner_minus[0]) && (x < rb_corner_minus[0])
        && (y > lt_corner_minus[1]) && (y < rb_corner_minus[1])) {
        return &minus_piston;
    }

    return NULL;
}

RendererError Renderer::DrawButtons() {
    Coordinates lt_corner(plus_piston.GetLTCorner());
    Coordinates rb_corner(plus_piston.GetRBCorner());

    sf::RectangleShape button_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
    button_background.setPosition(lt_corner[0], lt_corner[1]);
    if (plus_piston.GetPressedInfo()) {
        button_background.setFillColor(kPressedColor);
    } else {
        button_background.setFillColor(kReleaseColor);
    }

    sf::Font font;
    if (!font.loadFromFile(kFontFileName)) {
        return kCantLoadFont;
    }

    sf::Text text(plus_piston.GetText(), font, rb_corner[1] - lt_corner[1]);
    text.setPosition(lt_corner[0] + (rb_corner[0] - lt_corner[0]) * kShiftTextPlusHor,
                     lt_corner[1] - (rb_corner[1] - lt_corner[1]) * kShiftTextPlusVer);

    window.draw(button_background);
    window.draw(text);

    lt_corner = minus_piston.GetLTCorner();
    rb_corner = minus_piston.GetRBCorner();

    button_background.setSize(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
    button_background.setPosition(lt_corner[0], lt_corner[1]);
    if (minus_piston.GetPressedInfo()) {
        button_background.setFillColor(kPressedColor);
    } else {
        button_background.setFillColor(kReleaseColor);
    }

    text.setString(minus_piston.GetText());
    text.setPosition(lt_corner[0] + (rb_corner[0] - lt_corner[0]) * kShiftTextMinusHor,
                     lt_corner[1] - (rb_corner[1] - lt_corner[1]) * kShiftTextMinusVer);

    window.draw(button_background);
    window.draw(text);

    return kDoneRenderer;
}

RendererError Renderer::DrawReactor() {
    const Coordinates& lt_corner = reactor_manager.GetLTCorner();
    const Coordinates& rb_corner = reactor_manager.GetRBCorner();

    sf::RectangleShape reactor_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
    reactor_background.setPosition(lt_corner[0], lt_corner[1]);
    reactor_background.setFillColor(sf::Color::Cyan);

    window.draw(reactor_background);

    sf::RectangleShape piston(sf::Vector2f(kWidthPiston, rb_corner[1] - lt_corner[1]));
    piston.setPosition(reactor_manager.GetPistonX(), lt_corner[1]);
    piston.setFillColor(sf::Color::Red);

    window.draw(piston);

    MoveMolecules();

    DrawMolecules();

    CheckCollisions();

    CountEnergy();

    return kDoneRenderer;
}

RendererError Renderer::CheckCollisions() {
    std::vector<Object*>& objects = reactor_manager.GetObjectsVector();
    size_t objects_num = objects.size();

    typedef RendererError (*CheckCollision_t)(std::vector<Object*>& objects, size_t i, size_t j, bool& reaction);
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

        float coeff = sqrt((speed_first.SqLength() + speed_second.SqLength()) / 2);

        Object* res_object = NULL;
        try {
            res_object = new Cube((center_first + center_second) / 2, 2 * kCircleWeight,
                                  (!(speed_first + speed_second)) * coeff);
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

        float coeff = sqrt((speed_circle.SqLength() * kCircleWeight + speed_cube.SqLength() * weight_cube)
                            / (weight_cube + kCircleWeight));

        cube->SetCenterCoordinates((center_cube * weight_cube + center_circle * kCircleWeight) / (weight_cube + kCircleWeight));
        cube->SetSpeed((!(speed_cube * weight_cube + speed_circle * kCircleWeight)) * coeff);
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

RendererError Renderer::DrawMolecules() {
    std::vector<Object*>& objects = reactor_manager.GetObjectsVector();
    size_t objects_num = objects.size();

    const Coordinates& lt_corner = reactor_manager.GetLTCorner();
    const Coordinates& rb_corner = reactor_manager.GetRBCorner();
    float width = reactor_manager.GetPistonX() - lt_corner[0];
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

    return kDoneRenderer;
}

RendererError Renderer::DrawCube(const Object* const object) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinates();
    const Coordinates& lt_corner = reactor_manager.GetLTCorner();

    sf::RectangleShape rectangle(sf::Vector2f(kWidthCube, kWidthCube));
    rectangle.setPosition(sf::Vector2f(center[0] - kWidthCube / 2 + lt_corner[0],
                                       center[1] - kWidthCube / 2 + lt_corner[1]));
    rectangle.setFillColor(sf::Color::Red);
    window.draw(rectangle);

    return kDoneRenderer;
}

RendererError Renderer::DrawCircle(const Object* const object) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinates();
    const Coordinates& lt_corner = reactor_manager.GetLTCorner();
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

    return kDoneRenderer;
}

RendererError Renderer::MoveMolecules() {
    std::vector<Object*>& objects = reactor_manager.GetObjectsVector();
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

    return kDoneRenderer;
}

RendererError Renderer::MoveCircle(Object* const object, float distance) {
    ASSERT(object != NULL, "");

    Coordinates center = object->GetCenterCoordinates();
    Coordinates speed = object->GetSpeed();

    const Coordinates& lt_corner = reactor_manager.GetLTCorner();
    const Coordinates& rb_corner = reactor_manager.GetRBCorner();
    float width = reactor_manager.GetPistonX() - lt_corner[0];
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

    return kDoneRenderer;
}

RendererError Renderer::CountEnergy() {
    std::vector<Object*>& objects = reactor_manager.GetObjectsVector();
    size_t objects_num = objects.size();

    float energy = 0;
    for (size_t i = 0; i < objects_num; i++) {
        float speed_sq = objects[i]->GetSpeed().SqLength();
        enum ObjectType type = CheckObjectType(objects[i]->GetIDHash());
        float weight = (type == kCircleType) ? kCircleWeight : (dynamic_cast<Cube*>(objects[i]))->GetWeight();

        energy += weight * speed_sq / 2;
    }

    std::vector<float>& energies = graph_manager.GetValues();
    if (energies.size() == kMaxSizeExperiments) {
        energies.erase(energies.begin());
    }

    energies.push_back(energy);
    if (energy > graph_manager.GetMaxVal()) {
        graph_manager.SetMaxVal(energy);
    }
    if (energy < graph_manager.GetMinVal()) {
        graph_manager.SetMinVal(energy);
    }

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
