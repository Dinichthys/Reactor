#include "draw.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>

#include "graphics.hpp"

#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

RendererError UI::ShowWindow() {
    graphics::RectangleShape background((float)GetWidth(), (float)GetHeight());
    background.SetPosition(Coordinates(2, 0.f, 0.f));
    background.SetFillColor(graphics::kColorBlack);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto start_time = end_time;
    size_t duration = 0;

    graphics::Event event;
    while (window.IsOpen()) {
        if (window.PollEvent(event)) {
            if (event.GetType() == graphics::kClosed) {
                window.Close();
                break;
            }
            AnalyzeKey(event);
        }

        LOG(kDebug, "Drawing window");

        WidgetContainer::OnIdle();

        start_time = end_time;
        end_time = std::chrono::high_resolution_clock::now();
        duration += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        if (kOneSceneUpdateTimeInMicro <= duration) {
            duration = 0;

            window.Draw(background);

            WidgetContainer::Draw(&window);

            window.Display();
        }
    }

    return kDoneRenderer;
}

RendererError UI::AnalyzeKey(const graphics::Event& event) {
    static Coordinates mouse_pos(2, 0, 0);
    static Widget* moving_window = NULL;

    switch(event.GetType()) {
        case(graphics::kMouseButtonPressed) : {
            GetMousePosition(mouse_pos);

            OnMousePress(mouse_pos, &moving_window);

            LOG(kDebug, "Coordinates X = %f, Y = %f\n", mouse_pos[0], mouse_pos[1]);

            break;
        }
        case(graphics::kMouseButtonReleased) : {
            moving_window = NULL;

            GetMousePosition(mouse_pos);

            OnMouseRelease(mouse_pos);

            LOG(kDebug, "Coordinates X = %f, Y = %f\n", mouse_pos[0], mouse_pos[1]);

            break;
        }
        case(graphics::kMouseMoved) : {
            float old_x = mouse_pos[0];
            float old_y = mouse_pos[1];
            GetMousePosition(mouse_pos);

            OnMouseEnter(mouse_pos);

            if (moving_window == NULL) {
                break;
            }

            moving_window->OnMouseMove(mouse_pos[0] - old_x, mouse_pos[1] - old_y);

            break;
        }
        default:
            break;
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
