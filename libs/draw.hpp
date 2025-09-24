#ifndef DRAW_HPP
#define DRAW_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "reactor.hpp"
#include "graph.hpp"
#include "object.hpp"
#include "window.hpp"
#include "vector.hpp"

#include "my_assert.h"

const char* const kWindowName = "Reactor";
const size_t kTimeSleep = 10000;
const size_t kCharacterSize = 100;
const sf::Color kColorCircle = sf::Color::Green;

enum RendererError {
    kDoneRenderer = 0,
    kBadAllocReactionRenderer,
    kCantLoadFontRenderer,
};

class Renderer {
    private:
        sf::RenderWindow window;
        unsigned int screen_width;
        unsigned int screen_height;

        ReactorManager reactor_manager;
        GraphManager graph_manager;

        PistonButton plus_piston;
        PistonButton minus_piston;

    public:
        explicit Renderer(unsigned int width, unsigned int height,
                          const ReactorManager& reactor,
                          const GraphManager& graph,
                          const PistonButton& plus, const PistonButton& minus)
            : window(sf::VideoMode ({width, height}), kWindowName),
              reactor_manager(reactor),
              graph_manager(graph),
              plus_piston(plus), minus_piston(minus) {
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
        RendererError AnalyseKey(const sf::Event event);
        Window* IdentifyWindow(float x, float y);
        PistonButton* IdentifyButton(float x, float y);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
