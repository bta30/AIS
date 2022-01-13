#ifndef ARGS_H
#define ARGS_H

#include "img.h"
#include "align.h"
#include "interpolate.h"
#include "stack.h"

// Stores the arguments given to the program
typedef struct {
    char** fileNames;
    int fileNamesLen;
    int deleteFileNames;

    char* outputFileName;
    
    char *bias, *dark, *flat, *alignImage;

    alignFunc align;
    stackFunc stack;
    interpolationFunc interpolate;

    int alignChannel;
} programArgs;

// Generate a programArgs instance given the direct arguments to the program
int processArgs(int argc, char** argv, programArgs* args);

#endif