#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "img.h"

int calibrate(imgContainer* images, int imagesLen, char* bias, char* dark, char* flat);

#endif