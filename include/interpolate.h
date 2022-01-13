#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "img.h"
#include "vec.h"

typedef imgContainer (*interpolationFunc)(imgContainer image, alignInfo alignment);

imgContainer bilinearInterpolation(imgContainer image, alignInfo alignment);

#endif