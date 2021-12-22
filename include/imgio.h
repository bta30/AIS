#ifndef IMGIO_H
#define IMGIO_H

#include "img.h"

imgContainer loadImage(const char* filepath);
int exportImage(imgContainer image, const char* filepath);

#endif
