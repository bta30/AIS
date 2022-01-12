#include <stdio.h>

#include "imgPrivate.h"
#include "imgio.h"

int initImgContainer(imgContainer* img) {
    if(img->imageData == NULL && img->filePath == NULL) {
        fprintf(stderr, "%s\n", initNullError);
        return -1;
    } else if(img->imageData == NULL) {
        imgContainer newImage = loadImage(img->filePath);
        if(newImage.imageData == NULL) {
            return -1;
        }
        *img = newImage;
    }
}

int deinitImgContainer(imgContainer* img) {
    if(img->imageData == NULL) {
        fprintf(stderr, "%s%s\n", deinitNullImageError, img->imageData);
        return -1;
    }

    if(img->filePath == NULL) {
        fprintf(stderr, "%s\n", deinitNullFilePathError);
    }

    if(exportImage(*img, img->filePath)) {
        return -1;
    }

    free(img->imageData);
    img->imageData = NULL;

    return 0;
}