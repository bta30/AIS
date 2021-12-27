#ifndef CIRCLE_PATTERN_ALIGN_H
#define CIRCLE_PATTERN_ALIGN_H

#include "align.h"
#include "vec.h"

// Get the coordinates representing the centre of mass of the image
vec2 getCentreOfMass(imgContainer img);

// Get the sum of squares of differences between pixels of two images
double sumOfDiffSquares(imgContainer img1, imgContainer img2);

double getRotation(imgContainer reference, vec2 refCOM, imgContainer img, vec2 imgCOM);  // Get rough rotation of the image
double getRotationError(imgContainer reference, vec2 refCOM, imgContainer img, vec2 imgCOM, double angle);  // Get error from rotating by a certain angle

#endif