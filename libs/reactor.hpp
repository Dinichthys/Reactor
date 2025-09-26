#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <stdint.h>
#include <unistd.h>

class ReactorManager;

#include "button.hpp"
#include "object.hpp"
#include "vector.hpp"
#include "window.hpp"
#include "generate_obj.hpp"

static const float kWidthPiston = 5.f;
static const size_t kMaxSizeExperiments = 100000;
static const char* const kFontFileName = "data/font.ttf";
static const float  kShiftTextPlusVer = 0.2;
static const float  kShiftTextPlusHor = 0.2;
static const float  kShiftTextMinusVer = 0.2;
static const float  kShiftTextMinusHor = 0.3;
static const size_t kShowingButtonSleep = 10000;

enum ReactorError {
    kDoneReactor = 0,
    kBadAllocReaction,
    kCantLoadFont,
    kBadCast,
};

class ReactorManager : public Window {
    private:
        std::vector<Object*>& objects;

        float piston;

    public:
        explicit ReactorManager(const Coordinates& lt_corner, const Coordinates& rb_corner,
                                std::vector<Object*>& objects_val)
            :Window(lt_corner, rb_corner), objects(objects_val) {
            piston = rb_corner[0];
        };

        ~ReactorManager() {
            size_t objects_num = objects.size();
            for (size_t i = 0; i < objects_num; i++) {
                delete objects.back();
                objects.pop_back();
            }
        }

        std::vector<Object*>& GetObjectsVector() {return objects;};
        float GetPistonX() const {return piston;};
        void SetPistonX(float piston_x) {
            if (piston_x < piston / 2) {
                return;
            }
            piston = (piston_x < Window::GetRBCorner()[0]) ? piston_x : Window::GetRBCorner()[0];
        };
        virtual void SetRBCorner(const Coordinates& coors) override {
            float old_rb_x = Window::GetRBCorner()[0];
            Window::SetRBCorner(coors);
            piston = coors[0] - old_rb_x + piston;
        };

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            const Coordinates& lt_corner = Window::GetLTCorner();
            const Coordinates& rb_corner = Window::GetRBCorner();

            sf::RectangleShape reactor_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            reactor_background.setPosition(lt_corner[0], lt_corner[1]);
            reactor_background.setFillColor(sf::Color::Cyan);

            window->draw(reactor_background);

            sf::RectangleShape piston_pic(sf::Vector2f(kWidthPiston, rb_corner[1] - lt_corner[1]));
            piston_pic.setPosition(piston, lt_corner[1]);
            piston_pic.setFillColor(sf::Color::Red);

            window->draw(piston_pic);

            MoveMolecules();

            DrawMolecules(window);

            CheckCollisions();
        };

        virtual void Move(float shift_x, float shift_y) override {
            Window::Move(shift_x, shift_y);
            piston += shift_x;
        };

        float CountEnergy();

    private:
        ReactorError CheckCollisions();
        ReactorError DrawMolecules(sf::RenderWindow* window);
        ReactorError MoveMolecules();
        ReactorError DrawCircle(sf::RenderWindow* window, const Object* const object);
        ReactorError DrawCube(sf::RenderWindow* window, const Object* const object);
        ReactorError MoveCircle(Object* const object, float distance);
};

class PistonButton : public Button {
    private:
        float shift;

    public:
        explicit PistonButton(const Button& button, float shift_val)
            :Button(button) {
            shift = shift_val;
        };

        float GetShift() const {return shift;};

        virtual void Action(ReactorManager* reactor) override {
            ASSERT(reactor != NULL, "");

            bool pressed = Button::GetPressedInfo();
            Button::SetPressedInfo(!pressed);
            if (!pressed) {
                reactor->SetPistonX(reactor->GetPistonX() + shift);
            }
        };
};

class NumberMoleculeButton : public Button {
    private:
        int64_t delta;
        ObjectType type;

    public:
        explicit NumberMoleculeButton(const Button& button, float delta_val, ObjectType type_val)
            :Button(button) {
            delta = delta_val;
            type = type_val;
        };

        int64_t GetDelta() const {return delta;};
        ObjectType GetType() const {return type;};

        virtual void Action(ReactorManager* reactor) override {
            ASSERT(reactor != NULL, "");

            bool pressed = Button::GetPressedInfo();
            Button::SetPressedInfo(!pressed);
            if (pressed) {
                return;
            }

            if (delta > 0) {
                std::vector<Object*>& objects = reactor->GetObjectsVector();
                float height = reactor->GetRBCorner()[1] - reactor->GetLTCorner()[1];
                float width  = reactor->GetRBCorner()[0] - reactor->GetLTCorner()[0];
                switch(type) {
                    case kCircleType: {
                        GenerateCircleObjects(objects, width, height, delta);
                        return;
                    }
                    case kCubeType: {
                        GenerateCubeObjects(objects, width, height, delta);
                        return;
                    }
                    default:
                        return;
                }
            }

            int64_t deleted_num = - delta;

            std::vector<Object*>& objects = reactor->GetObjectsVector();
            size_t objects_num = objects.size();
            for(size_t i = 0; (i < objects_num) && (deleted_num > 0); i++) {
                if (objects[i]->GetType() == type) {
                    delete objects[i];
                    objects.erase(objects.begin() + i);
                    objects_num--;
                    i--;
                    deleted_num--;
                }
            }
        };
};

#endif // REACTOR_HPP
