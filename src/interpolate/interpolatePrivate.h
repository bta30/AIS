#ifndef INTERPOLATE_PRIVATE_H
#define INTERPOLATE_PRIVATE_H

#include "interpolate.h"

typedef struct {
    double x, y;
} vec2;

vec2 transform(imgAlignment img, int x, int y);  // Get aligned coords given old coords
vec2 untransform(imgAlignment img, int x, int y);  // Get old coords given aligned coords

imgContainer alignImage(imgAlignment img, double (*interpolate)(imgContainer img, int channel, vec2 coords));  // Generate aligned image using given interpolation algorithm

double bilinear(imgContainer img, int channel, vec2 coords);

#endif