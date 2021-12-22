#ifndef STACK_H
#define STACK_H

#include "img.h"

typedef imgContainer (*stackFunc)(imgContainer* images, int imagesLen);

imgContainer meanStack(imgContainer* images, int imagesLen);

#endif