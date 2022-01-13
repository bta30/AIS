#include <stdlib.h>
#include <stdio.h>

#include "calibratePrivate.h"
#include "imgio.h"

int calibrate(imgContainer* images, int imagesLen, char* bias, char* dark, char* flat) {
    if(bias == NULL && dark == NULL && flat == NULL) {
        return 0;
    }

    imgContainer denominator = {NULL, 0, 0, 0, NULL, 0};
    if(bias != NULL && flat != NULL) {
        denominator = getDenominator(bias, flat);
        if(denominator.imageData == NULL) {
            return -1;
        }
    }
    
    imgContainer darkFrame = {NULL, 0, 0, 0, NULL, 0};
    if(dark != NULL) {
        imgContainer darkFrame = loadImage(dark);
        if(darkFrame.imageData == NULL) {
            return -1;
        }
    }

    performCalibration(images, imagesLen, darkFrame, denominator);
    return 0;
}

calibrationInfo getCalibration(char* bias, char* dark, char* flat) {
    calibrationInfo calibration = {{NULL, 0, 0, 0, NULL, 0},
                                    {NULL, 0, 0, 0, NULL, 0},
                                    0};

    if(bias == NULL && dark == NULL && flat == NULL) {
        return calibration;
    }

    imgContainer denominator = {NULL, 0, 0, 0, NULL, 0};
    if(bias != NULL && flat != NULL) {
        denominator = getDenominator(bias, flat);
        if(denominator.imageData == NULL) {
            calibration.error = -1;
            return calibration;
        }
    }
    
    imgContainer darkFrame = {NULL, 0, 0, 0, NULL, 0};
    if(dark != NULL) {
        imgContainer darkFrame = loadImage(dark);
        if(darkFrame.imageData == NULL) {
            calibration.error = -1;
            return calibration;
        }
    }

    calibration.dark = darkFrame;
    calibration.denominator = denominator;
    return calibration;
}

imgContainer getDenominator(char* bias, char* flat) {
    // Load bias frame
    imgContainer biasFrame = {NULL, 0, 0, 0, bias, 0};
    if(bias != NULL) {
        biasFrame = loadImage(bias);
        if(biasFrame.imageData == NULL) {
            return (imgContainer){NULL, 0, 0, 0, NULL, 0};
        }
    }

    // Load flat frame
    imgContainer flatFrame = {NULL, 0, 0, 0, bias, 0};
    if(flat != NULL) {
        flatFrame = loadImage(flat);
        if(biasFrame.imageData != NULL) {
            subtractImage(flatFrame, biasFrame);
        }
    } else if(biasFrame.imageData != NULL) {
        flatFrame = createFilled1Image(biasFrame.width, biasFrame.height, biasFrame.channels);
    }

    // Return normalised calibration frame if it exists
    if(flatFrame.imageData == NULL) {
        return (imgContainer){NULL, 0, 0, 0, NULL, 0};
    } else {
        normaliseMean(flatFrame);
        return flatFrame;
    }
}

void performCalibration(imgContainer* images, int imagesLen, imgContainer dark, imgContainer denominator) {
    for(int i = 0; i < imagesLen; i++) {
        initImgContainer(images + i);
        if(dark.imageData != NULL) {
            subtractImage(images[i], dark);
        }

        if(denominator.imageData != NULL) {
            divideImage(images[i], denominator);
        }

        normaliseMax(images[i]);
        deinitImgContainer(images + i);
    }
}

void calibrateImage(imgContainer image, calibrationInfo calibration) {
    if(calibration.dark.imageData != NULL) {
        subtractImage(image, calibration.dark);
    }

    if(calibration.denominator.imageData != NULL) {
        divideImage(image, calibration.denominator);
    }

    normaliseMax(image);
}

imgContainer createFilled1Image(int width, int height, int channels) {
    imgContainer img = {malloc(width * height * channels * sizeof(double)),
                            width, height, channels};
    if(img.imageData == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return (imgContainer){NULL, 0, 0, 0, NULL, 0};
    }

    for(int i = 0; i < width * height * channels; i++) {
        img.imageData[i] = 1.0;
    }

    return img;
}

void subtractImage(imgContainer img1, imgContainer img2) {
    for(int i = 0; i < img1.width * img1.height * img1.channels; i++) {
        img1.imageData[i] -= img2.imageData[i];
    }
}

void divideImage(imgContainer img1, imgContainer img2) {
    for(int i = 0; i < img1.width * img1.height * img1.channels; i++) {
        img1.imageData[i] /= img2.imageData[i];
    }
}

void normaliseMean(imgContainer img) {
    for(int c = 0; c < img.channels; c++) {
        double pixelSum = 0.0;
        for(int i = 0; i < img.width * img.height;  i++) {
            pixelSum += img.imageData[c * img.width * img.height + i];
        }
        double pixelAvg = pixelSum / (img.width * img.height);
        for(int i = 0; i < img.width * img.height;  i++) {
            img.imageData[c * img.width * img.height + i] /= pixelAvg;
        }
    }
}

void normaliseMax(imgContainer img) {
    for(int c = 0; c < img.channels; c++) {
        double pixelMax = 0.0;
        for(int i = 0; i < img.width * img.height;  i++) {
            double value = img.imageData[c * img.width * img.height + i];
            if(pixelMax < value) {
                pixelMax = value;
            }
        }
        for(int i = 0; i < img.width * img.height;  i++) {
            img.imageData[c * img.width * img.height + i] /= pixelMax;
        }
    }
}