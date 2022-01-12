#include <math.h>

#include "vec.h"

vec2 subtract(vec2 x, vec2 y) {
    return (vec2){x.x - y.x, x.y - y.y};
}

double diffSquared(vec2 x, vec2 y) {
    double dx = x.x - y.x;
    double dy = x.y - y.y;
    return dx * dx + dy * dy;
}

vec2 rotate(vec2 v, double angle) {
    double rotCos = cos(angle);
    double rotSin = sin(angle);
    return (vec2) {rotCos * v.x - rotSin *  v.y,
                    rotSin * v.x + rotCos * v.y};
}