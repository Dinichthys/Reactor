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
    sf::RectangleShape background(sf::Vector2f((float)GetWidth(), (float)GetHeight()));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color::Black);

    sf::Event event;
    while (window.isOpen()) {
        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            AnalyseKey(event);
        }

        WidgetContainer::OnIdle();

        LOG(kDebug, "Drawing window");

        window.draw(background);

        WidgetContainer::Draw(&window);

        usleep(kTimeSleep);

        window.display();
    }

    return kDoneRenderer;
}

RendererError UI::AnalyseKey(const sf::Event event) {

    if (event.type == sf::Event::MouseButtonPressed) {
        int mouse_x = 0;
        int mouse_y = 0;

        GetMousePosition(&mouse_x, &mouse_y);

        WidgetContainer::OnMousePress(Coordinates(2, (float)mouse_x, (float)mouse_y));

        LOG(kError, "Coordinates X = %f, Y = %f\n", (float)mouse_x, (float)mouse_y);

        return kDoneRenderer;
    }

    return kDoneRenderer;
}

// RendererError UI::AnalyseKey(const sf::Event event) {
//     static bool moving = false;
//     static int mouse_x = 0;
//     static int mouse_y = 0;
//     static Window* moving_window = NULL;
//
//     if (event.type == sf::Event::MouseButtonPressed) {
//         if (moving) {
//             moving = false;
//             mouse_x = 0;
//             mouse_y = 0;
//
//             return kDoneRenderer;
//         }
//
//         GetMousePosition(&mouse_x, &mouse_y);
//         Button* pressed_button = IdentifyButton(mouse_x, mouse_y);
//         if(pressed_button != NULL) {
//             mouse_x = 0;
//             mouse_y = 0;
//             pressed_button->Action(&reactor_manager);
//             return kDoneRenderer;
//         }
//
//         moving = true;
//         moving_window = IdentifyWindow(mouse_x, mouse_y);
//         if (moving_window != NULL) {
//             return kDoneRenderer;
//         }
//         moving = false;
//         mouse_x = 0;
//         mouse_y = 0;
//
//         return kDoneRenderer;
//     }
//
//     if ((moving) && (event.type == sf::Event::MouseMoved)) {
//         int old_x = mouse_x;
//         int old_y = mouse_y;
//         GetMousePosition(&mouse_x, &mouse_y);
//
//         moving_window->Move((float)(mouse_x - old_x), (float)(mouse_y - old_y));
//     }
//
//     if (event.type == sf::Event::MouseButtonReleased) {
//         if (moving) {
//             moving = false;
//             mouse_x = 0;
//             mouse_y = 0;
//
//             return kDoneRenderer;
//         }
//
//         GetMousePosition(&mouse_x, &mouse_y);
//         Button* pressed_button = IdentifyButton(mouse_x, mouse_y);
//         if(pressed_button != NULL) {
//             mouse_x = 0;
//             mouse_y = 0;
//             pressed_button->Action(&reactor_manager);
//             return kDoneRenderer;
//         }
//
//         moving = true;
//         moving_window = IdentifyWindow(mouse_x, mouse_y);
//         if (moving_window != NULL) {
//             return kDoneRenderer;
//         }
//         moving = false;
//         mouse_x = 0;
//         mouse_y = 0;
//
//         return kDoneRenderer;
//     }
//
//     return kDoneRenderer;
// }

void UI::GetMousePosition(int* mouse_x, int* mouse_y) {
    ASSERT(mouse_x != NULL, "");
    ASSERT(mouse_y != NULL, "");

    float scale_x = ((float)window.getSize().x / (float)kStartWidth);
    float scale_y = ((float)window.getSize().y / (float)kStartHeight);
    *mouse_x = (int)((float)sf::Mouse::getPosition(window).x / scale_x);
    *mouse_y = (int)((float)sf::Mouse::getPosition(window).y / scale_y);
}

// Window* UI::IdentifyWindow(float x, float y) {
//     const Coordinates& lt_corner_reactor = reactor_manager.GetLTCorner();
//     const Coordinates& rb_corner_reactor = reactor_manager.GetRBCorner();
//     if ((x > lt_corner_reactor[0]) && (x < rb_corner_reactor[0])
//         && (y > lt_corner_reactor[1]) && (y < rb_corner_reactor[1])) {
//         return &reactor_manager;
//     }
//
//     const Coordinates& lt_corner_graph = graph_manager.GetLTCorner();
//     const Coordinates& rb_corner_graph = graph_manager.GetRBCorner();
//     if ((x > lt_corner_graph[0]) && (x < rb_corner_graph[0])
//         && (y > lt_corner_graph[1]) && (y < rb_corner_graph[1])) {
//         return &graph_manager;
//     }
//
//     const Coordinates& lt_corner_panel = panel_control.GetLTCorner();
//     const Coordinates& rb_corner_panel = panel_control.GetRBCorner();
//     if ((x > lt_corner_panel[0]) && (x < rb_corner_panel[0])
//         && (y > lt_corner_panel[1]) && (y < rb_corner_panel[1])) {
//         return &panel_control;
//     }
//
//     return NULL;
// }
//
// Button* UI::IdentifyButton(float x, float y) {
//     std::vector<Button*>& buttons = panel_control.GetButtons();
//     size_t buttons_num = buttons.size();
//     for (size_t i = 0; i < buttons_num; i++) {
//         const Coordinates& lt_corner_plus = buttons[i]->GetLTCorner();
//         const Coordinates& rb_corner_plus = buttons[i]->GetRBCorner();
//         if ((x > lt_corner_plus[0]) && (x < rb_corner_plus[0])
//             && (y > lt_corner_plus[1]) && (y < rb_corner_plus[1])) {
//             return buttons[i];
//         }
//     }
//
//     return NULL;
// }

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
