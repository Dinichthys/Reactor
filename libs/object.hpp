#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "graphics.hpp"

#include "vector.hpp"
#include "window.hpp"

static const float kWidthCube = 5;
static const float kCircleRadius = 5;
static const size_t kCircleWeight = 1;

static const graphics::Color kColorCircle = graphics::kColorGreen;
static const graphics::Color kColorCube = graphics::kColorRed;

enum ObjectType {
    kNone = -1,

    kCircleType = 0,
    kCubeType,

    kObjectsNum,
};

class Object : public Widget {
    private:
        enum ObjectType type_;
        Coordinates speed_;

    public:
        explicit Object(const Coordinates& lt_corner, float width, float height,
                        const Coordinates& speed, const enum ObjectType type, Widget* parent = NULL)
            :Widget(lt_corner, width, height, parent), speed_(speed) {
            type_ = type;
        };

        virtual ~Object() {};

        Coordinates GetCenterCoordinatesLoc() const {
            return Widget::GetLTCornerLoc() + Coordinates(2, Widget::GetWidth() / 2, Widget::GetHeight() / 2);
        };
        Coordinates GetCenterCoordinatesAbs() const {
            return Widget::GetLTCornerAbs() + Coordinates(2, Widget::GetWidth() / 2, Widget::GetHeight() / 2);
        };

        virtual Coordinates GetSpeed() const {return Coordinates(speed_);};
        virtual enum ObjectType GetType() const {return type_;};
        virtual void SetSpeed(Coordinates speed) {speed_ = speed;};
        virtual void SetType(enum ObjectType type) {type_ = type;};
};

class Cube : virtual public Object {
    private:
        size_t weight;

    public:
        explicit Cube(const Coordinates& center, size_t weight_val, const Coordinates& speed_val, Widget* parent = NULL)
            :Object(center - Coordinates(2, kWidthCube, kWidthCube) / 2, kWidthCube, kWidthCube, speed_val, kCubeType, parent) {
            weight = weight_val;
        };

        virtual ~Cube() {};

        size_t GetWeight() const {return weight;};
        void   SetWeight(size_t new_weight) {weight = new_weight;};

        virtual void Draw(graphics::RenderWindow* window) {
            ASSERT(window != NULL, "");

            const Coordinates lt_corner(Widget::GetLTCornerAbs());

            graphics::RectangleShape rectangle(kWidthCube, kWidthCube);
            rectangle.SetPosition(lt_corner);
            rectangle.SetFillColor(kColorCube);
            window->Draw(rectangle);
        };
};

class Circle : virtual public Object {
    public:
        explicit Circle(const Coordinates& center, const Coordinates& speed_val, Widget* parent = NULL)
            :Object(center - Coordinates(2, kCircleRadius, kCircleRadius), kCircleRadius, kCircleRadius, speed_val, kCircleType, parent) {};

        virtual ~Circle() {};

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates center = Object::GetCenterCoordinatesAbs();
            graphics::VertexArray vertices ((size_t) (4 * kCircleRadius * kCircleRadius));
            size_t vertex_index = 0;
            for (float i = -kCircleRadius; i < kCircleRadius; i++) {
                for (float j = -kCircleRadius; j < kCircleRadius; j++) {
                    if (i * i + j * j < kCircleRadius * kCircleRadius) {
                        vertices.SetPixelPosition(vertex_index, Coordinates(2, center[0] + j, center[1] + i));
                        vertices.SetPixelColor(vertex_index, kColorCircle);
                        vertex_index++;
                    }
                }
            }

            window->Draw(vertices);
        }

};

#endif // OBJECT_HPP
