#ifndef MY_WINDOW_HPP
#define MY_WINDOW_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

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

        virtual void Draw(sf::RenderWindow& window) = 0;
};

#endif // MY_WINDOW_HPP
