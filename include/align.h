#ifndef ALIGN_H
#define ALIGN_H

#include "img.h"

typedef imgAlignment* (*alignFunc)(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel);

imgAlignment* noAlign(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel);
imgAlignment* starThresholdAlign(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel);

#endif