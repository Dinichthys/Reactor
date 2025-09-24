#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <stdint.h>
#include <vector>
#include <float.h>

#include "vector.hpp"
#include "window.hpp"

const float kAxisWidth = 2.f;
const float kStep      = 1.f;
const float kGridWidth = 1.f;
const int8_t kGridBrightness = 63;
const float kMinDifferenceScale = 1.1f;
const float kAxisNum = 10.f;

enum GraphError {
    kDoneGraph = 0,
};

class GraphManager : public Window  {
    private:
        std::vector<float> points;

        float max = FLT_MIN;
        float min = FLT_MAX;

    public:
        explicit GraphManager(const Coordinates& lt_corner, const Coordinates& rb_corner)
            :Window(lt_corner, rb_corner) {};

        std::vector<float>& GetValues() {return points;};
        float GetMaxVal() {return max;};
        float GetMinVal() {return min;};

        void SetMaxVal(float new_max) {max = new_max;};
        void SetMinVal(float new_min) {min = new_min;};

        virtual void Draw(sf::RenderWindow& window) override;
        GraphError DrawAxis(sf::RenderWindow& window);
        GraphError DrawGraph(sf::RenderWindow& window);
};

#endif // GRAPH_HPP
