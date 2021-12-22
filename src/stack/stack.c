#include <stdlib.h>
#include <math.h>

#include "stack.h"

imgContainer meanStack(imgContainer* images, int imagesLen) {
    // Create container for stacked images
    int imageDataLen = images[0].width * images[0].height * images[0].channels;
    imgContainer stackedImage = images[0];
    stackedImage.imageData = malloc(imageDataLen * sizeof(double));
    int* pixelCounts = malloc(imageDataLen * sizeof(int));

    // Add all images together
    for(int i = 0; i < imagesLen; i++) {
        for(int j = 0; j < imageDataLen && !isnan(images[i].imageData[j]); j++) {
            stackedImage.imageData[j] += images[i].imageData[j];
            pixelCounts[i]++;
        }
    }

    // Divide by number of images
    for(int i = 0; i < imageDataLen; i++) {
        stackedImage.imageData[i] /= pixelCounts[i];
    }

    free(pixelCounts);
    return stackedImage;
}