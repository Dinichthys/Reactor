#ifndef EVENT_HPP
#define EVENT_HPP

#include "window.hpp"

enum EventType {
    kIdol = 0,
    kMouseMove,
    kMousePress,
    kMouseEnter,
};

class Event {
    private:
        enum EventType type_;

    public:
        explicit Event(enum EventType type) {
            type_ = type;
        };

        virtual void DispatchEvent(Widget* root) = 0;
};

class MousePress : public Event {
    private:
        Coordinates mouse_pos_;

    public:
        explicit MousePress(const Coordinates& mouse_pos)
            :Event(kMousePress), mouse_pos_(mouse_pos) {};
};

#endif // EVENT_HPP
