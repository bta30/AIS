#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "imgPrivate.h"
#include "imgio.h"

int initImgContainer(imgContainer* img) {
    if(img->imageData == NULL && img->filePath == NULL) {
        fprintf(stderr, "%s\n", initNullError);
        return -1;
    } else if(img->imageData == NULL) {
        imgContainer newImage = loadImage(img->filePath);
        newImage.toDeleteFilePath = img->toDeleteFilePath;
        if(newImage.imageData == NULL) {
            return -1;
        }
        *img = newImage;
    }
}

int deinitImgContainer(imgContainer* img) {
    if(img->imageData == NULL) {
        fprintf(stderr, "%s%s\n", deinitNullImageError, img->filePath);
        return -1;
    }

    if(img->filePath == NULL) {
        fprintf(stderr, "%s\n", deinitNullFilePathError);
    }

    ensureTempDir(img);
    if(exportImage(*img, img->filePath)) {
        return -1;
    }

    free(img->imageData);
    img->imageData = NULL;

    return 0;
}

void ensureTempDir(imgContainer* img) {
    // Check if the path is in tempDir
    int tempDirLength = strlen(tempDir);
    if(strncmp(img->filePath, tempDir, tempDirLength) != 0) {
        // Get the name of the file
        char* lastSlash = strrchr(img->filePath, '/');
        char* fileName = (lastSlash == NULL) ? img->filePath : lastSlash + 1;

        int newLen = strlen(fileName) + tempDirLength + 1;
        char* newFilePath = malloc(newLen * sizeof(char));
        strcpy(newFilePath, tempDir);
        strcat(newFilePath, fileName);

        if(img->toDeleteFilePath) {
            free(img->filePath);
        }
        img->filePath = newFilePath;
    }
}