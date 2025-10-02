#ifndef REACTOR_BUTTON_HPP
#define REACTOR_BUTTON_HPP

#include "window.hpp"
#include "reactor.hpp"

class ReactorButton : public Button {
    public:
        explicit ReactorButton(const Button& button)
            :Button(button) {};

        virtual void Action(Reactor* reactor) = 0;

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            ASSERT(widget != NULL, "");

            *widget = NULL;
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            LOG(kDebug, "Coordinates On Button X = %f, Y = %f\n"
                        "Coordinates On Button X = %f, Y = %f\n"
                        "Width = %f, Height = %f\n", mouse_pos[0], mouse_pos[1], lt_corner[0], lt_corner[1], width, height);

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                Button::SetPressedInfo(true);
                Action(dynamic_cast<Reactor*>(dynamic_cast<WidgetContainer*>(Widget::GetParent()->GetParent())->GetChild(kReactor)));

                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                Button::SetPressedInfo(false);

                return true;
            }

            return false;
        };
};

class PistonButton : public ReactorButton {
    private:
        float shift_;

    public:
        explicit PistonButton(const Button& button, float shift)
            :ReactorButton(button) {
            shift_ = shift;
        };

        float GetShift() const {return shift_;};

        virtual void Action(Reactor* reactor) override {
            ASSERT(reactor != NULL, "");

            bool pressed = Button::GetPressedInfo();
            if (pressed) {
                reactor->SetPistonX(reactor->GetPistonX() + shift_);
            }
        };
};

class NumberMoleculeButton : public ReactorButton {
    private:
        int64_t delta_;
        ObjectType type_;

    public:
        explicit NumberMoleculeButton(const Button& button, float delta, ObjectType type)
            :ReactorButton(button) {
            delta_ = delta;
            type_ = type;
        };

        int64_t GetDelta() const {return delta_;};
        ObjectType GetType() const {return type_;};

        virtual void Action(Reactor* reactor) override {
            ASSERT(reactor != NULL, "");

            bool pressed = Button::GetPressedInfo();
            if (!pressed) {
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
};

#endif // REACTOR_BUTTON_HPP
