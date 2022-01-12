#ifndef IMG_H
#define IMG_H

// Stores information of a single image
typedef struct {
	double* imageData;
	int width, height, channels;
    char* filePath;
} imgContainer;

// Stores alignment information of a single image
typedef struct {
    imgContainer image;
    double dx, dy, angle;
} imgAlignment;

int initImgContainer(imgContainer* img);
int deinitImgContainer(imgContainer* img);

#endif