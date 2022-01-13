#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "img.h"
#include "vec.h"

typedef imgContainer (*interpolationFunc)(imgAlignment img);

imgContainer bilinearInterpolation(imgAlignment img);

#endif