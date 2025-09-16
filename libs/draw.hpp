#ifndef DRAW_HPP
#define DRAW_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "graph.hpp"
#include "circle.hpp"
#include "vector.hpp"
#include "cubes.hpp"

#include "my_assert.h"

const char* const kWindowName = "Reactor";
const size_t kTimeSleep = 50000;
const sf::Color kColorCircle = sf::Color::Green;

enum RendererError {
    kDoneRenderer = 0,
};

class Reactor {
    private:
        const Coordinates lt_corner;
        const Coordinates rb_corner;

    public:
        explicit Reactor(Coordinates lt_corner_val, Coordinates rb_corner_val)
            :lt_corner(lt_corner_val), rb_corner(rb_corner_val) {};

        const Coordinates& GetLTCorner() const {return lt_corner;};
        const Coordinates& GetRBCorner() const {return rb_corner;};
};

class SceneManager {
    private:
        std::vector<Circle>& circles;
        std::vector<Cube>& cubes;

        Reactor reactor;

    public:
        explicit SceneManager(std::vector<Circle>& circles_val, std::vector<Cube>& cubes_val,
                              const Reactor& reactor_val)
            :circles(circles_val), cubes(cubes_val), reactor(reactor_val) {};

        std::vector<Circle>& GetCircleVector() const {return circles;};
        std::vector<Cube>& GetCubesVector() const {return cubes;};
        const Reactor& GetReactor() const {return reactor;};
};

class Renderer {
    private:
        sf::RenderWindow window;
        unsigned int screen_width;
        unsigned int screen_height;

        SceneManager scene_manager;

    public:
        explicit Renderer(unsigned int width, unsigned int height,
                          std::vector<Circle>& circles,
                          std::vector<Cube>& cubes,
                          const Reactor& reactor)
            : window(sf::VideoMode ({width, height}), kWindowName),
              scene_manager(circles, cubes, reactor) {
            screen_width = width;
            screen_height = height;
        };
        ~Renderer() {
            if (window.isOpen()) {
                window.close();
            }

            screen_width = 0;
            screen_height = 0;
        };

        RendererError ShowWindow();

    private:
        RendererError DrawCircles(const Circle& circle);
        RendererError CheckCollisions();
        RendererError CheckBorders();
};

#endif // DRAW_HPP
