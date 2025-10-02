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

enum RendererError {
    kDoneRenderer = 0,
    kBadAllocReactionRenderer,
    kCantLoadFontRenderer,
};

class UI : public WidgetContainer {
    private:
        sf::RenderWindow window;

    public:
        explicit UI(unsigned int width, unsigned int height,
                     const std::vector<Widget*>& children)
            : WidgetContainer(Coordinates(2, 0, 0), width, height),
              window(sf::VideoMode ({width, height}), kWindowName) {
            std::vector<Widget*>& children_ = WidgetContainer::GetChildren();

            size_t children_num = children.size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back(children[i]);
            }
        };
        ~UI() {
            if (window.isOpen()) {
                window.close();
            }
        };

        RendererError ShowWindow();

    private:
        RendererError AnalyseKey(const sf::Event event);
        void GetMousePosition(int* mouse_x, int* mouse_y);
        Widget* IdentifyWindow(float x, float y);
        Button* IdentifyButton(float x, float y);
        RendererError PistonButtonAction(Button* piston_button);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
