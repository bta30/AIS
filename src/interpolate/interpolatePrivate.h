#ifndef INTERPOLATE_PRIVATE_H
#define INTERPOLATE_PRIVATE_H

#include "interpolate.h"

vec2 untransform(alignInfo alignment, int x, int y);  // Get old coords given aligned coords

imgContainer alignImage(imgContainer image, alignInfo alignment, double (*interpolate)(imgContainer image, int channel, vec2 coords));  // Generate aligned image using given interpolation algorithm

#endif