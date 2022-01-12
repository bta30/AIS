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
        for(int j = 0; j < imageDataLen; j++) {
            if(!isnan(images[i].imageData[j])) {
                stackedImage.imageData[j] += images[i].imageData[j];
                pixelCounts[j]++;
            }
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