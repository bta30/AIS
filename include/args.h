#ifndef ARGS_H
#define ARGS_H

#include "img.h"
#include "align.h"
#include "stack.h"

// Stores the arguments given to the program
typedef struct {
    char** fileNames;
    int fileNamesLen;
    int deleteFileNames;

    char *bias, *dark, *flat, *alignImage;

    alignFunc align;
    stackFunc stack;

    int alignChannel;
} programArgs;

// Generate a programArgs instance given the direct arguments to the program
int processArgs(int argc, char** argv, programArgs* args);

#endif