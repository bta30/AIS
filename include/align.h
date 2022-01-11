#ifndef ALIGN_H
#define ALIGN_H

#include "img.h"

typedef imgAlignment* (*alignFunc)(imgContainer reference, imgContainer* images, int imagesLen);

imgAlignment* noAlign(imgContainer reference, imgContainer* images, int imagesLen);
imgAlignment* circlePatternAlign(imgContainer reference, imgContainer* images, int imagesLen);
imgAlignment* starThresholdAlign(imgContainer reference, imgContainer* images, int imagesLen);

#endif