#include <math.h>
#include <stdlib.h>

#include "interpolatePrivate.h"

vec2 untransform(alignInfo alignment, int x, int y) {
    double rotCos = cos(alignment.angle);
    double rotSin = sin(alignment.angle);
    double xt = x - alignment.dx;
    double yt = y - alignment.dy;
    return (vec2){rotCos * xt + rotSin * yt,
                    -rotSin * xt + rotCos * yt};
}

imgContainer alignImage(imgContainer image, alignInfo alignment, double (*interpolate)(imgContainer image, int channel, vec2 coords)) {
    // Create container for interpolated image
    int imageDataLen = image.width * image.height * image.channels;
    imgContainer stackedImage = image;
    stackedImage.imageData = malloc(imageDataLen * sizeof(double));
    vec2 topLeft = untransform(alignment, 0, 0);
    vec2 diff = subtract(untransform(alignment, 1, 1), topLeft);
    
    // Get interpolated value for each pixel
    for(int y = 0; y < stackedImage.height; y++) {
        for(int x = 0; x < stackedImage.width; x++) {
            for(int i = 0; i < stackedImage.channels; i++) {
                int dataIndex = stackedImage.width * stackedImage.height * i + stackedImage.width * y + x;
                vec2 coords = (vec2){topLeft.x + x * diff.x, topLeft.y + y * diff.y};
                double interpolatedValue;
                if(coords.x >= 0 && coords.x <= stackedImage.width - 1 &&
                    coords.y >= 0 && coords.y <= stackedImage.height - 1) {
                    interpolatedValue = interpolate(image, i, coords);
                } else {
                    interpolatedValue = 0.0;
                }
                stackedImage.imageData[dataIndex] = interpolatedValue;
            }
        }
    }

    return stackedImage;
}