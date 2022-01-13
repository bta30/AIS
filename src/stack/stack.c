#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

imgContainer meanStack(imgContainer* images, int imagesLen) {
    // Create container for stacked images
    int initialisedStacked = 0;
    imgContainer stackedImage;
    int imageDataLen;
    int* pixelCounts;

    // Go through all images
    for(int i = 0; i < imagesLen; i++) {
        if(images[i].imageData != NULL || images[i].filePath != NULL) {
            if(images[i].filePath == NULL) {
                printf("Stacking next image\n");
            } else {
                printf("Stacking image %s\n", images[i].filePath);
            }
            initImgContainer(images + i);

            // Initialise stacking data if needed
            if(!initialisedStacked) {
                imageDataLen = images[i].width * images[i].height * images[i].channels;
                stackedImage = images[i];
                stackedImage.imageData = (double*)calloc(imageDataLen, sizeof(double));
                pixelCounts = (int*)calloc(imageDataLen, sizeof(int));
                initialisedStacked = 1;
            }

            // Add the current image to the average
            for(int j = 0; j < imageDataLen; j++) {
                if(images[i].imageData[j] != 0.0) {
                    stackedImage.imageData[j] += images[i].imageData[j];
                    pixelCounts[j]++;
                }
            }
            deinitImgContainer(images + i);
        }
    }

    // Divide by number of images
    for(int i = 0; i < imageDataLen; i++) {
        if(pixelCounts[i] == 0) {
            stackedImage.imageData[i] = 0;
        } else {
            stackedImage.imageData[i] /= pixelCounts[i];
        }
    }

    free(pixelCounts);
    return stackedImage;
}

imgContainer meanSigmaRejectStack(imgContainer* images, int imagesLen) {
    // Create container for stacked images
    imgContainer stackedImage;
    int initialisedStacked = 0;
    double* sumX;
    double* sumX2;
    double* mean;
    double* sd;
    int imageDataLen;
    int* pixelCounts;

    // Calculate sum x and sum x^2 for each pixel
    printf("Calculated sum x and sum x^2\n");
    for(int i = 0; i < imagesLen; i++) {
        if(images[i].imageData != NULL || images[i].filePath != NULL) {
            if(images[i].filePath == NULL) {
                printf("Stacking next image\n");
            } else {
                printf("Stacking image %s\n", images[i].filePath);
            }
            initImgContainer(images + i);

            // Initialise stacking data if needed
            if(!initialisedStacked) {
                imageDataLen = images[i].width * images[i].height * images[i].channels;
                stackedImage = (imgContainer){NULL, images[i].width, images[i].height, images[i].channels, NULL, 0};
                sumX = (double*)calloc(imageDataLen, sizeof(double));
                sumX2 = (double*)calloc(imageDataLen, sizeof(double));
                mean = (double*)calloc(imageDataLen,  sizeof(double));
                sd = (double*)calloc(imageDataLen,  sizeof(double));
                pixelCounts = (int*)calloc(imageDataLen, sizeof(int));
                initialisedStacked = 1;
            }

            // Add the current image to the average
            for(int j = 0; j < imageDataLen; j++) {
                if(images[i].imageData[j] != 0.0) {
                    sumX[j] += images[i].imageData[j];
                    sumX2[j] += images[i].imageData[j] * images[i].imageData[j];
                    pixelCounts[j]++;
                }
            }
            deinitImgContainer(images + i);
        }
    }

    // Calculate standard deviation for each pixel
    printf("Calculating standard deviation\n");
    for(int i = 0; i < imageDataLen; i++) {
        if(pixelCounts[i] == 0) {
            sd[i] = -1.0;
        } else {
            mean[i] = sumX[i] / pixelCounts[i];
            double variance = sumX2[i] / pixelCounts[i] - mean[i]*mean[i];
            sd[i] = sqrt(variance);
        }
    }
    free(sumX2);
    memset(sumX, 0, imageDataLen * sizeof(double));
    memset(pixelCounts, 0, imageDataLen * sizeof(int));

    // Getting new average of pixels
    printf("Calculating mean of pixels\n");
    stackedImage.imageData = sumX;
    for(int i = 0; i < imagesLen; i++) {
        if(images[i].imageData != NULL || images[i].filePath != NULL) {
            if(images[i].filePath == NULL) {
                printf("Stacking next image\n");
            } else {
                printf("Stacking image %s\n", images[i].filePath);
            }
            initImgContainer(images + i);

            // Add the current image to the average
            for(int j = 0; j < imageDataLen; j++) {
                if(images[i].imageData[j] != 0.0 && sd[j] != -1.0 && fabs(images[i].imageData[j] - mean[j]) < 2*sd[j]){
                    stackedImage.imageData[j] += images[i].imageData[j];
                    pixelCounts[j]++;
                }
            }
            deinitImgContainer(images + i);
        }
    }
    free(mean);
    free(sd);

    // Divide by number of images
    for(int i = 0; i < imageDataLen; i++) {
        if(pixelCounts[i] == 0) {
            stackedImage.imageData[i] = 0;
        } else {
            stackedImage.imageData[i] /= pixelCounts[i];
        }
    }
    free(pixelCounts);

    return stackedImage;
}