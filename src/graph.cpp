#include "graph.hpp"

#include <stdint.h>
#include <math.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"
#include "draw.hpp"

#include "logging.h"
#include "my_assert.h"

Graph::Graph(Coordinates lt_corner_val, Coordinates rb_corner_val, Coordinates start_val, float scale_val)
    :lt_corner(lt_corner_val), rb_corner(rb_corner_val), start_point(start_val) {
    ASSERT(!isnan(scale_val), "Invalid scale for graph");
    scale = scale_val;
}
