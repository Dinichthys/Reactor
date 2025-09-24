#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>

#include "object.hpp"
#include "vector.hpp"
#include "window.hpp"
#include "button.hpp"

const float kWidthPiston = 5.f;
const size_t kMaxSizeExperiments = 100000;
const char* const kFontFileName = "data/font.ttf";
const float  kShiftTextPlusVer = 0.2;
const float  kShiftTextPlusHor = 0.2;
const float  kShiftTextMinusVer = 0.2;
const float  kShiftTextMinusHor = 0.3;

enum ReactorError {
    kDoneReactor = 0,
    kBadAllocReaction,
    kCantLoadFont,
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

        std::vector<Object*>& GetObjectsVector() const {return objects;};
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

        virtual void Draw(sf::RenderWindow& window) override {
            const Coordinates& lt_corner = Window::GetLTCorner();
            const Coordinates& rb_corner = Window::GetRBCorner();

            sf::RectangleShape reactor_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            reactor_background.setPosition(lt_corner[0], lt_corner[1]);
            reactor_background.setFillColor(sf::Color::Cyan);

            window.draw(reactor_background);

            sf::RectangleShape piston_pic(sf::Vector2f(kWidthPiston, rb_corner[1] - lt_corner[1]));
            piston_pic.setPosition(piston, lt_corner[1]);
            piston_pic.setFillColor(sf::Color::Red);

            window.draw(piston_pic);

            MoveMolecules();

            DrawMolecules(window);

            CheckCollisions();
        };

        ReactorError CheckCollisions();
        ReactorError DrawMolecules(sf::RenderWindow& window);
        ReactorError MoveMolecules();
        ReactorError DrawCircle(sf::RenderWindow& window, const Object* const object);
        ReactorError DrawCube(sf::RenderWindow& window, const Object* const object);
        ReactorError MoveCircle(Object* const object, float distance);

        float CountEnergy();
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

        virtual void Draw(sf::RenderWindow& window) override {
            Coordinates lt_corner(Window::GetLTCorner());
            Coordinates rb_corner(Window::GetRBCorner());

            sf::RectangleShape button_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            button_background.setPosition(lt_corner[0], lt_corner[1]);
            if (Button::GetPressedInfo()) {
                button_background.setFillColor(kPressedColor);
            } else {
                button_background.setFillColor(kReleaseColor);
            }

            sf::Font font;
            if (!font.loadFromFile(kFontFileName)) {
                return;
            }

            sf::Text text(Button::GetText(), font, rb_corner[1] - lt_corner[1]);
            text.setPosition(lt_corner[0] + (rb_corner[0] - lt_corner[0]) * kShiftTextPlusHor,
                            lt_corner[1] - (rb_corner[1] - lt_corner[1]) * kShiftTextPlusVer);

            window.draw(button_background);
            window.draw(text);
        };
};

#endif // REACTOR_HPP
