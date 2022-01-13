#ifndef ALIGN_H
#define ALIGN_H

#include "img.h"
#include "calibrate.h"
#include "interpolate.h"

typedef imgContainer* (*alignFunc)(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel, calibrationInfo calibration, interpolationFunc interpolate);

imgContainer* noAlign(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel, calibrationInfo calibration, interpolationFunc interpolate);
imgContainer* starThresholdAlign(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel, calibrationInfo calibration, interpolationFunc interpolate);

#endif