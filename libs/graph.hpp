#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <stdint.h>
#include <vector>
#include <float.h>

#include "graphics.hpp"

#include "vector.hpp"
#include "window.hpp"

static const float kWidthGraph = 500;
static const float kHeightGraph = 400;
static const int kMaxBrightness = 255;
static const float kAxisWidth = 2.f;
static const float kStep = 1.f;
static const float kGridWidth = 1.f;
static const int8_t kGridBrightness = 63;
static const float kMinDifferenceScale = 1.1f;
static const float kAxisNum = 10.f;

enum GraphError {
    kDoneGraph = 0,
};

class GraphManager : public Widget  {
    private:
        std::vector<float> points;

        float max_ = FLT_MIN;
        float min_ = FLT_MAX;

    public:
        explicit GraphManager(const Coordinates& lt_corner, float width, float height, Widget* parent = NULL)
            :Widget(lt_corner, width, height, parent) {};

        std::vector<float>& GetValues() {return points;};
        float GetMaxVal() {return max_;};
        float GetMinVal() {return min_;};

        void SetMaxVal(float new_max) {max_ = new_max;};
        void SetMinVal(float new_min) {min_ = new_min;};

        virtual void Draw(graphics::RenderWindow* window) override;
        GraphError DrawAxis(graphics::RenderWindow* window);
        GraphError DrawGraph(graphics::RenderWindow* window);
};

#endif // GRAPH_HPP
