#ifndef CALIBRATE_PRIVATE_H
#define CALIBRATE_PRIVATE_H

#include "calibrate.h"
#include "errors.h"

void performCalibration(imgContainer* images, int imagesLen, imgContainer dark, imgContainer denominator);

imgContainer getDenominator(char* bias, char* flat);
imgContainer createFilled1Image(int width, int height, int channels);

void subtractImage(imgContainer img1, imgContainer img2); 
void divideImage(imgContainer img1, imgContainer img2);
void normaliseMean(imgContainer img);
void normaliseMax(imgContainer img);

#endif