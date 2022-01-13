#include <stdlib.h>
#include <stdio.h>

#include "args.h"
#include "calibrate.h"
#include "imgio.h"
#include "align.h"
#include "interpolate.h"
#include "stack.h"

imgContainer stackNoAlign(imgContainer* images, programArgs args) {
    printf("Calibrating images\n");
    if(calibrate(images, args.fileNamesLen, args.bias, args.dark, args.flat)) {
        return (imgContainer){NULL, 0, 0, 0, NULL, 0};
    }

    printf("Stacking images\n");
    imgContainer stacked = args.stack(images, args.fileNamesLen);
    
    for(int i = 0; i <  args.fileNamesLen; i++) {
        free(images[i].imageData);
    }
    free(images);

    return stacked;
}

imgContainer stackAlign(imgContainer* images, programArgs args) {
    printf("Getting image calibration\n");
    calibrationInfo calibration = getCalibration(args.bias, args.dark, args.flat);
    if(calibration.error) {
        return (imgContainer){NULL, 0, 0, 0, NULL, 0};
    }

    printf("Aligning images\n");
    imgContainer alignImage = loadImage(args.alignImage);
    imgContainer* interpolated = args.align(alignImage, images, args.fileNamesLen, args.alignChannel, calibration, args.interpolate);
    free(images);

    printf("Stacking images\n");
    imgContainer stacked = args.stack(interpolated, args.fileNamesLen);

    for(int i = 0; i < args.fileNamesLen; i++) {
        if(interpolated[i].imageData != NULL) {
            free(interpolated[i].imageData);
        }
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

    imgContainer stacked = (args.align == noAlign) ? stackNoAlign(images, args) : stackAlign(images, args);
    if(stacked.imageData == NULL) {
        return -1;
    }

    printf("Exporting output image: %s\n", args.outputFileName);
    exportImage(stacked, args.outputFileName);
    printf("Exported output image\n");

    if(args.deleteFileNames) {
        free(args.fileNames);
    }

    return 0;
}