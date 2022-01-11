#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "img.h"
#include "vec.h"

imgContainer bilinearInterpolation(imgAlignment img);
imgContainer bilinearInterpolationWindow(imgContainer img, double angle, vec2 centre, int windowWidth);

#endif