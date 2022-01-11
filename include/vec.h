#ifndef VEC_H
#define VEC_H

typedef struct {
    double x, y;
} vec2;

vec2 subtract(vec2 x, vec2 y);
double diffSquared(vec2 x, vec2 y);
#endif