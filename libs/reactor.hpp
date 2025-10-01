#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <stdint.h>
#include <unistd.h>

class Reactor;

#include "button.hpp"
#include "object.hpp"
#include "vector.hpp"
#include "window.hpp"
#include "generate_obj.hpp"
#include "graph.hpp"
#include "logging.h"

static const float kWidthReactor = 500;
static const float kHeightReactor = 400;
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

enum ReactorManagerChildrenIndexes {
    kReactor = 0,
    kGraphManager = 1,
    kPanelControl = 2,
};

class Reactor : public WidgetContainer {
    private:
        float piston_;

    public:
        explicit Reactor(const Coordinates& lt_corner, float width, float height,
                         const std::vector<Widget*>& objects, Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height) {
            piston_ = width;

            std::vector<Widget*>& objects_ = WidgetContainer::GetChildren();
            size_t length = objects.size();
            for (size_t i = 0; i < length; i++) {
                objects_.push_back(objects[i]);
            }

            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        float GetPistonX() const {return piston_;};
        void SetPistonX(float piston) {
            if (piston < piston_ / 2) {
                return;
            }

            float width = Widget::GetWidth();
            piston_ = (piston < width) ? piston : width;
        };

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            WidgetContainer::SetParentToChildren();

            LOG(kDebug, "Draw Reactor");

            const Coordinates lt_corner(Widget::GetLTCornerAbs());
            const Coordinates rb_corner(Widget::GetRBCornerAbs());

            sf::RectangleShape reactor_background(sf::Vector2f(rb_corner[0] - lt_corner[0], rb_corner[1] - lt_corner[1]));
            reactor_background.setPosition(lt_corner[0], lt_corner[1]);
            reactor_background.setFillColor(sf::Color::Cyan);

            window->draw(reactor_background);

            DrawMolecules(window);

            sf::RectangleShape piston_pic(sf::Vector2f(kWidthPiston, rb_corner[1] - lt_corner[1]));
            piston_pic.setPosition(piston_ + lt_corner[0], lt_corner[1]);
            piston_pic.setFillColor(sf::Color::Red);

            window->draw(piston_pic);
        };

        virtual bool OnIdle() override {
            CheckCollisions();

            MoveMolecules();

            WidgetContainer::SetParentToChildren();

            return true;
        };

        float CountEnergy();

    private:
        ReactorError CheckCollisions();
        ReactorError DrawMolecules(sf::RenderWindow* window);
        ReactorError MoveMolecules();
        ReactorError MoveCircle(Object* const object, float distance);
};

class ReactorManager : public WidgetContainer {
    public:
        explicit ReactorManager(const Coordinates& lt_corner, float width, float height,
                                const std::vector<Widget*>& objects,
                                const GraphManager& graph_manager,
                                const PanelControl& panel_control,
                                Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height) {

            std::vector<Widget*>& children = WidgetContainer::GetChildren();
            children.push_back(new(std::nothrow) Reactor(Coordinates(2, 0, 0), width, height, objects, parent));
            if (children.back() == NULL) {
                throw std::runtime_error("Bad allocation for reactor manager");
            }
            children.push_back(new(std::nothrow) GraphManager(graph_manager));
            if (children.back() == NULL) {
                throw std::runtime_error("Bad allocation for reactor manager");
            }
            children.push_back(new(std::nothrow) PanelControl(panel_control));
            if (children.back() == NULL) {
                throw std::runtime_error("Bad allocation for reactor manager");
            }

            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        Reactor* GetReactor() {return dynamic_cast<Reactor*>(WidgetContainer::GetChildren()[kReactor]);};
        GraphManager* GetGraphManager() {return dynamic_cast<GraphManager*>(WidgetContainer::GetChildren()[kGraphManager]);};
        PanelControl* GetPanelControl() {return dynamic_cast<PanelControl*>(WidgetContainer::GetChildren()[kPanelControl]);};

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            LOG(kDebug, "Drawing Reactor Manager");

            WidgetContainer::Draw(window);
        };

        virtual bool OnIdle() override {
            WidgetContainer::OnIdle();

            float energy = GetReactor()->CountEnergy();
            GraphManager* graph_manager = GetGraphManager();
            std::vector<float>& energies = graph_manager->GetValues();
            if (energies.size() == kMaxSizeExperiments) {
                energies.erase(energies.begin());
            }

            energies.push_back(energy);
            if (energy > graph_manager->GetMaxVal()) {
                graph_manager->SetMaxVal(energy);
            }
            if (energy < graph_manager->GetMinVal()) {
                graph_manager->SetMinVal(energy);
            }

            return true;
        };
};

class PistonButton : public Button {
    private:
        float shift_;

    public:
        explicit PistonButton(const Button& button, float shift)
            :Button(button) {
            shift_ = shift;
        };

        float GetShift() const {return shift_;};

        virtual void Action(Reactor* reactor) {
            ASSERT(reactor != NULL, "");

            bool pressed = Button::GetPressedInfo();
            Button::SetPressedInfo(!pressed);
            if (!pressed) {
                reactor->SetPistonX(reactor->GetPistonX() + shift_);
            }
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            LOG(kError, "Coordinates On Button X = %f, Y = %f\n"
                        "Coordinates On Button X = %f, Y = %f\n"
                        "Width = %f, Height = %f\n", mouse_pos[0], mouse_pos[1], lt_corner[0], lt_corner[1], width, height);

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                Action(dynamic_cast<Reactor*>(dynamic_cast<WidgetContainer*>(Widget::GetParent()->GetParent())->GetChild(kReactor)));

                return true;
            }

            return false;
        };
};

class NumberMoleculeButton : public Button {
    private:
        int64_t delta_;
        ObjectType type_;

    public:
        explicit NumberMoleculeButton(const Button& button, float delta, ObjectType type)
            :Button(button) {
            delta_ = delta;
            type_ = type;
        };

        int64_t GetDelta() const {return delta_;};
        ObjectType GetType() const {return type_;};

        virtual void Action(Reactor* reactor) {
            ASSERT(reactor != NULL, "");

            bool pressed = Button::GetPressedInfo();
            Button::SetPressedInfo(!pressed);
            if (pressed) {
                return;
            }

            if (delta_ > 0) {
                std::vector<Widget*>& objects = reactor->GetChildren();
                float height = reactor->GetHeight();
                float width  = reactor->GetWidth();
                switch(type_) {
                    case kCircleType: {
                        GenerateCircleObjects(objects, width, height, delta_, this);
                        return;
                    }
                    case kCubeType: {
                        GenerateCubeObjects(objects, width, height, delta_, this);
                        return;
                    }
                    default:
                        return;
                }
            }

            int64_t deleted_num = - delta_;

            std::vector<Widget*>& objects = reactor->GetChildren();
            size_t objects_num = objects.size();
            for(size_t i = 0; (i < objects_num) && (deleted_num > 0); i++) {
                if (dynamic_cast<Object*>(objects[i])->GetType() == type_) {
                    delete objects[i];
                    objects.erase(objects.begin() + i);
                    objects_num--;
                    i--;
                    deleted_num--;
                }
            }
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                Action(dynamic_cast<Reactor*>(dynamic_cast<WidgetContainer*>(Widget::GetParent()->GetParent())->GetChild(kReactor)));

                return true;
            }

            return false;
        };
};

#endif // REACTOR_HPP
