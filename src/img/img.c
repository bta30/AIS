#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "imgPrivate.h"
#include "imgio.h"

int initImgContainer(imgContainer* image) {
    if(image->imageData == NULL && image->filePath == NULL) {
        fprintf(stderr, "%s\n", initNullError);
        return -1;
    } else if(image->imageData == NULL) {
        imgContainer newImage = loadImage(image->filePath);
        newImage.toDeleteFilePath = image->toDeleteFilePath;
        if(newImage.imageData == NULL) {
            return -1;
        }
        *image = newImage;
    }
}

int deinitImgContainer(imgContainer* image) {
    if(image->imageData == NULL) {
        fprintf(stderr, "%s%s\n", deinitNullImageError, image->filePath);
        return -1;
    }

    if(image->filePath == NULL) {
        fprintf(stderr, "%s\n", deinitNullFilePathError);
    }

    ensureTempDir(image);
    if(exportImage(*image, image->filePath)) {
        return -1;
    }

    free(image->imageData);
    image->imageData = NULL;

    return 0;
}

void ensureTempDir(imgContainer* image) {
    // Check if the path is in tempDir
    int tempDirLength = strlen(tempDir);
    if(strncmp(image->filePath, tempDir, tempDirLength) != 0) {
        // Get the name of the file
        char* lastSlash = strrchr(image->filePath, '/');
        char* fileName = (lastSlash == NULL) ? image->filePath : lastSlash + 1;

        int newLen = strlen(fileName) + tempDirLength + 1;
        char* newFilePath = malloc(newLen * sizeof(char));
        strcpy(newFilePath, tempDir);
        strcat(newFilePath, fileName);

        if(image->toDeleteFilePath) {
            free(image->filePath);
        }
        image->filePath = newFilePath;
    }
}