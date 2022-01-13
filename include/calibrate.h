#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "img.h"

// Stores all relevant information needed to calibrate images
typedef struct {
    imgContainer dark;
    imgContainer denominator;
    int error;
} calibrationInfo;

int calibrate(imgContainer* images, int imagesLen, char* bias, char* dark, char* flat);
calibrationInfo getCalibration(char* bias, char* dark, char* flat);
void calibrateImage(imgContainer image, calibrationInfo calibration);

#endif