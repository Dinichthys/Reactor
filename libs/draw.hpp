#ifndef DRAW_HPP
#define DRAW_HPP

#include "graphics.hpp"

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
        graphics::RenderWindow window;

    public:
        explicit UI(unsigned int width, unsigned int height,
                     const std::vector<Widget*>& children)
            :WidgetContainer(Coordinates(2, 0, 0), width, height),
              window(width, height, kWindowName) {
            std::vector<Widget*>& children_ = WidgetContainer::GetChildren();

            size_t children_num = children.size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back(children[i]);
            }

            WidgetContainer::SetParentToChildren();
        };
        ~UI() {
            if (window.IsOpen()) {
                window.Close();
            }
        };

        RendererError ShowWindow();

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            ASSERT(widget != NULL, "");

            WidgetContainer::OnMousePress(mouse_pos, widget);

            *widget = (*widget == this) ? NULL : *widget;

            return false;
        };

    private:
        RendererError AnalyzeKey(const graphics::Event& event);
        void GetMousePosition(Coordinates& mouse_pos);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
