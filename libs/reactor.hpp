#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <stdint.h>
#include <unistd.h>
#include <stdexcept>

#include "graphics.hpp"

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

        graphics::RectangleShape reactor_background_;
        graphics::RectangleShape piston_background_;

    public:
        explicit Reactor(const Coordinates& lt_corner, float width, float height,
                         const std::vector<Widget*>& objects, Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height), reactor_background_(width, height),
             piston_background_(kWidthPiston, height) {
            reactor_background_.SetFillColor(graphics::kColorCyan);
            piston_background_.SetFillColor(graphics::kColorRed);

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

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            WidgetContainer::SetParentToChildren();

            LOG(kDebug, "Draw Reactor");

            const Coordinates lt_corner(Widget::GetLTCornerAbs());

            reactor_background_.SetPosition(lt_corner);
            window->Draw(reactor_background_);

            DrawMolecules(window);

            piston_background_.SetPosition(Coordinates(2, piston_ + lt_corner[0], lt_corner[1]));
            window->Draw(piston_background_);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            ASSERT(widget != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                *widget = this;
                return true;
            }

            return false;
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
        ReactorError DrawMolecules(graphics::RenderWindow* window);
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

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            LOG(kDebug, "Drawing Reactor Manager");

            WidgetContainer::Draw(window);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            ASSERT(widget != NULL, "");

            bool result = WidgetContainer::OnMousePress(mouse_pos, widget);

            *widget = (*widget == this) ? NULL : *widget;

            return result;
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

#endif // REACTOR_HPP
