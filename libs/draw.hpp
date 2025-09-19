#ifndef DRAW_HPP
#define DRAW_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "graph.hpp"
#include "object.hpp"
#include "vector.hpp"

#include "my_assert.h"

const char* const kWindowName = "Reactor";
const size_t kTimeSleep = 25000;
const sf::Color kColorCircle = sf::Color::Green;

enum RendererError {
    kDoneRenderer = 0,
    kBadAllocReaction,
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
        std::vector<Object*>& objects;

        Reactor reactor;

    public:
        explicit SceneManager(std::vector<Object*>& objects_val, const Reactor& reactor_val)
            :objects(objects_val), reactor(reactor_val) {};

        ~SceneManager() {
            size_t objects_num = objects.size();
            for (size_t i = 0; i < objects_num; i++) {
                const Object* object = objects.back();
                size_t object_hash_code = object->GetIDHash();
                if (object_hash_code == kCircleIDHashCode) {
                    delete dynamic_cast<Circle*>(objects.back());
                }
                if (object_hash_code == kCubeIDHashCode) {
                    delete dynamic_cast<Cube*>(objects.back());
                }
                objects.pop_back();
            }
        }

        std::vector<Object*>& GetObjectsVector() const {return objects;};
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
                          std::vector<Object*>& objects,
                          const Reactor& reactor)
            : window(sf::VideoMode ({width, height}), kWindowName),
              scene_manager(objects, reactor) {
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
        RendererError DrawCircle(const Object* const object);
        RendererError DrawCube(const Object* const object);
        RendererError MoveCircle(Object* const object);
        RendererError MoveCube(Object* const object);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
