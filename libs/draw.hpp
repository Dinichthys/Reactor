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

static const unsigned int kStartHeight = 720;
static const unsigned int kStartWidth = 1080;
static const char* const kWindowName = "Reactor";
static const size_t kTimeSleep = 10000;
static const size_t kCharacterSize = 100;
static const sf::Color kColorCircle = sf::Color::Green;

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

        PanelControl panel_control;

    public:
        explicit Renderer(unsigned int width, unsigned int height,
                          const ReactorManager& reactor,
                          const GraphManager& graph,
                          const PanelControl& panel)
            : window(sf::VideoMode ({width, height}), kWindowName),
              reactor_manager(reactor),
              graph_manager(graph),
              panel_control(panel) {
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

        ReactorManager& GetReactorManager() {return reactor_manager;};

    private:
        RendererError AnalyseKey(const sf::Event event);
        void GetMousePosition(int* mouse_x, int* mouse_y);
        Window* IdentifyWindow(float x, float y);
        Button* IdentifyButton(float x, float y);
        RendererError PistonButtonAction(Button* piston_button);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
