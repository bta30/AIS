#ifndef IMG_H
#define IMG_H

// Stores information of a single image
typedef struct {
	double* imageData;
	int width, height, channels;
} imgContainer;

// Stores alignment information of a single image
typedef struct {
    imgContainer image;
    double dx, dy, angle;
} imgAlignment;

#endif