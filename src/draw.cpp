#include "draw.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

RendererError UI::ShowWindow() {
    graphics::RectangleShape background((float)GetWidth(), (float)GetHeight());
    background.SetPosition(Coordinates(2, 0.f, 0.f));
    background.SetFillColor(graphics::kColorBlack);

    sf::Event event;
    while (window.IsOpen()) {
        if (window.PollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.Close();
                break;
            }
            AnalyzeKey(event);
        }

        WidgetContainer::OnIdle();

        LOG(kDebug, "Drawing window");

        window.Draw(background);

        WidgetContainer::Draw(&window);

        usleep(kTimeSleep);

        window.Display();
    }

    return kDoneRenderer;
}
#include "clock.hpp"
RendererError UI::AnalyzeKey(const sf::Event event) {
    static Coordinates mouse_pos(2, 0, 0);
    static Widget* moving_window = NULL;

    if (event.type == sf::Event::MouseButtonPressed) {
        GetMousePosition(mouse_pos);

        OnMousePress(mouse_pos, &moving_window);

        LOG(kError, "Coordinates X = %f, Y = %f\n", mouse_pos[0], mouse_pos[1]);

        return kDoneRenderer;
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        moving_window = NULL;

        GetMousePosition(mouse_pos);

        OnMouseRelease(mouse_pos);

        LOG(kError, "Coordinates X = %f, Y = %f\n", mouse_pos[0], mouse_pos[1]);

        return kDoneRenderer;
    }

    if ((moving_window != NULL) && (event.type == sf::Event::MouseMoved)) {
        float old_x = mouse_pos[0];
        float old_y = mouse_pos[1];
        GetMousePosition(mouse_pos);

        if (dynamic_cast<Reactor*>(moving_window)) {
            fprintf(stderr, "Reactor");
        } else
        if (dynamic_cast<UI*>(moving_window)) {
            fprintf(stderr, "UI");
        } else
        if (dynamic_cast<PanelControl*>(moving_window)) {
            fprintf(stderr, "PanelControl");
        } else
        if (dynamic_cast<Button*>(moving_window)) {
            fprintf(stderr, "Button");
        } else
        if (dynamic_cast<Clock*>(moving_window)) {
            fprintf(stderr, "Clock");
        } else {
            fprintf(stderr, "------");
        }

        moving_window->OnMouseMove(mouse_pos[0] - old_x, mouse_pos[1] - old_y);
    }


    return kDoneRenderer;
}

void UI::GetMousePosition(Coordinates& mouse_pos) {
    mouse_pos = window.GetMousePos();
}

const char* ErrorHandler(enum RendererError error) {
    switch (error) {
        case kDoneRenderer:
            return "All were done";
        case kBadAllocReactionRenderer:
            return "Can't allocate memory in reaction";
        default:
            return "Invalid enum RendererError identifier";
    }
}
