#include <stdlib.h>
#include <stdio.h>

#include "calibratePrivate.h"
#include "imgio.h"

int calibrate(imgContainer* images, int imagesLen, char* bias, char* dark, char* flat) {
    if(bias == NULL && dark == NULL && flat == NULL) {
        return 0;
    }

    imgContainer denominator = {NULL, 0, 0, 0, NULL};
    if(bias != NULL && flat != NULL) {
        denominator = getDenominator(images[0].width, images[0].height, images[0].channels, bias, flat);
        if(denominator.imageData == NULL) {
            return -1;
        }
    }
    
    imgContainer darkFrame = {NULL, 0, 0, 0, NULL};
    if(dark != NULL) {
        imgContainer darkFrame = loadImage(dark);
        if(darkFrame.imageData == NULL) {
            return -1;
        }
    }

    performCalibration(images, imagesLen, darkFrame, denominator);
    return 0;
}

imgContainer getDenominator(int width, int height, int channels, char* bias, char* flat) {
    imgContainer flatFrame = (flat != NULL) ? loadImage(flat) :
        createFilled1Image(width, height, channels);
    if(flatFrame.imageData == NULL) {
        return (imgContainer){NULL, 0, 0, 0, NULL};
    }
    if(flat != NULL) {
        normaliseMean(flatFrame);
    }

    if(bias != NULL) {
        imgContainer biasFrame = loadImage(bias);
        if(biasFrame.imageData == NULL) {
            return (imgContainer){NULL, 0, 0, 0, NULL};
        }
        subtractImage(flatFrame, biasFrame);
    }

    return flatFrame;
}

void performCalibration(imgContainer* images, int imagesLen, imgContainer dark, imgContainer denominator) {
    for(int i = 0; i < imagesLen; i++) {
        if(dark.imageData != NULL) {
            subtractImage(images[i], dark);
        }

        if(denominator.imageData != NULL) {
            divideImage(images[i], denominator);
        }

        normaliseMax(images[i]);
    }
}

imgContainer createFilled1Image(int width, int height, int channels) {
    imgContainer img = {malloc(width * height * channels * sizeof(double)),
                            width, height, channels};
    if(img.imageData == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return (imgContainer){NULL, 0, 0, 0, NULL};
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