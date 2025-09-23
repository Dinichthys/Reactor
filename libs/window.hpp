#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "vector.hpp"

class Window {
    private:
        Coordinates lt_corner;
        Coordinates rb_corner;

    public:
        explicit Window(const Coordinates& lt_corner_val, const Coordinates& rb_corner_val)
            :lt_corner(lt_corner_val), rb_corner(rb_corner_val) {};

        virtual const Coordinates& GetLTCorner() const {return lt_corner;};
        virtual const Coordinates& GetRBCorner() const {return rb_corner;};

        virtual void SetLTCorner(const Coordinates& coors) {lt_corner = coors;};
        virtual void SetRBCorner(const Coordinates& coors) {rb_corner = coors;};
};

#endif // WINDOW_HPP
