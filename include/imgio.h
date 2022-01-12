#ifndef IMGIO_H
#define IMGIO_H

#include "img.h"

imgContainer loadImage(char* filePath);
imgContainer* loadImages(char** fileNames, int fileNamesLen);
int exportImage(imgContainer image, const char* filePath);

#endif
