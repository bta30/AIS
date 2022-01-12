#include <stdlib.h>
#include <stdio.h>

#include "args.h"
#include "calibrate.h"
#include "imgio.h"
#include "align.h"
#include "interpolate.h"
#include "stack.h"

imgContainer stackNoAlign(imgContainer* images, programArgs args) {
    printf("Stacking images\n");
    imgContainer stacked = args.stack(images, args.fileNamesLen);
    
    for(int i = 0; i <  args.fileNamesLen; i++) {
        free(images[i].imageData);
    }
    free(images);

    return stacked;
}

imgContainer stackAlign(imgContainer* images, programArgs args) {
    printf("Aligning images\n");
    imgContainer alignImage = loadImage(args.alignImage);
    imgAlignment* aligned = args.align(alignImage, images, args.fileNamesLen, args.alignChannel);
    free(images);

    printf("Interpolating images\n");
    int alignedNum = 0;
    imgContainer* interpolated = malloc(args.fileNamesLen * sizeof(imgContainer));
    for(int i = 0; i < args.fileNamesLen; i++) {
        if(aligned[i].dx != 0.0 &&
            aligned[i].dy != 0.0 &&
            aligned[i].angle != 0.0) {
                interpolated[alignedNum++] = args.interpolate(aligned[i]);
        }
        free(aligned[i].image.imageData);
    }
    free(aligned);

    printf("Stackings images\n");
    imgContainer stacked = args.stack(interpolated, alignedNum);

    for(int i = 0; i < alignedNum; i++) {
        free(interpolated[i].imageData);
    }
    free(interpolated);
    return stacked;
}

int main(int argc, char** argv) {
    printf("Processing program arguments\n");
    int err;
    programArgs args;
    if(err = processArgs(argc, argv, &args)) {
        return (err == 1) ? 0 : err;
    }
    printf("Processed program arguments\n");

    printf("Loading images\n");
    imgContainer* images = loadImages(args.fileNames, args.fileNamesLen);
    if(images == NULL) {
        return -1;
    }

    printf("Calibrating images\n");
    if(err = calibrate(images, args.fileNamesLen, args.bias, args.dark, args.flat)) {
        return err;
    }

    imgContainer stacked = (args.align == noAlign) ? stackNoAlign(images, args) : stackAlign(images, args);

    printf("Exporting output image: %s\n", args.outputFileName);
    exportImage(stacked, args.outputFileName);
    printf("Exported output image\n");

    if(args.deleteFileNames) {
        free(args.fileNames);
    }

    return 0;
}