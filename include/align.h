#ifndef ALIGN_H
#define ALIGN_H

#include "img.h"

typedef imgAlignment (*alignFunc)(imgContainer reference, imgContainer image);

imgAlignment noAlign(imgContainer reference, imgContainer image);
imgAlignment forwardAdditive(imgContainer reference, imgContainer image);

#endif