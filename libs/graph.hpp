#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <stdint.h>

#include "vector.hpp"

const float kAxisWidth = 2.f;
const float kStep      = 1.f;
const float kGridWidth = 1.f;
const int8_t kGridBrightness = 63;

class Graph {
    private:
        Coordinates lt_corner;
        Coordinates rb_corner;

        Coordinates start_point;

        float scale;

    public:
        explicit Graph(Coordinates lt_corner_val, Coordinates rb_corner_val, Coordinates start_val, float scale_val);

        Coordinates GetLTCoordinates() const {return Coordinates(lt_corner);};
        Coordinates GetRBCoordinates() const {return Coordinates(rb_corner);};
        Coordinates GetStartCoordinates() const {return Coordinates(start_point);};
        float GetScale() const {return scale;};
};

#endif // GRAPH_HPP
