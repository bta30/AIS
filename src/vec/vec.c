#include "vec.h"

vec2 subtract(vec2 x, vec2 y) {
    return (vec2){x.x - y.x, x.y - y.y};
}

double diffSquared(vec2 x, vec2 y) {
    double dx = x.x - y.x;
    double dy = x.y - y.y;
    return dx * dx + dy * dy;
}