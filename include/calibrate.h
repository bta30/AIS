#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "img.h"

imgContainer calibrate(imgContainer image, char* bias, char* dark, char* flat);

#endif