#ifndef IMG_PRIVATE_H
#define IMG_PRIVATE_H

#include "img.h"

const char* initNullError = "Error: Attempted to initialise an imgContainer with NULL imageData and filePath";
const char* deinitNullImageError = "Error: Attempted to deinitialise an imgContainer with NULL imageData, with filePath: ";
const char* deinitNullFilePathError = "Error: Attempted to deinitialise an imgContainer with NULL filePath";

const char* tempDir = "temp/";

void ensureTempDir(imgContainer* image);  // If the path of image is not in tempDir, replace it with a new path in tempDir

#endif