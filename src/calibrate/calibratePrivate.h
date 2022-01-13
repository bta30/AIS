#ifndef CALIBRATE_PRIVATE_H
#define CALIBRATE_PRIVATE_H

#include "calibrate.h"
#include "errors.h"

void performCalibration(imgContainer* images, int imagesLen, imgContainer dark, imgContainer denominator);

imgContainer getDenominator(char* bias, char* flat);
imgContainer createFilled1Image(int width, int height, int channels);

void subtractImage(imgContainer image1, imgContainer image2); 
void divideImage(imgContainer image1, imgContainer image2);
void normaliseMean(imgContainer image);
void normaliseMax(imgContainer image);

#endif