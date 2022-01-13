#ifndef IMG_H
#define IMG_H

// Stores information of a single image
typedef struct {
	double* imageData;
	int width, height, channels;
    char* filePath;
    int toDeleteFilePath;
} imgContainer;

// Stores alignment information for a single image
typedef struct {
    double dx, dy, angle;
} alignInfo;

int initImgContainer(imgContainer* image);
int deinitImgContainer(imgContainer* image);

#endif