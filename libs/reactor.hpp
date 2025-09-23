#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>

#include "object.hpp"
#include "vector.hpp"
#include "window.hpp"
#include "button.hpp"

const float kWidthPiston = 5.f;
const size_t kMaxSizeExperiments = 100000;

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
};

#endif // REACTOR_HPP
