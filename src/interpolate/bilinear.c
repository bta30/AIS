#include <math.h>

#include "bilinear.h"

imgContainer bilinearInterpolation(imgContainer image, alignInfo alignment) {
    imgContainer outImage = alignImage(image, alignment, bilinear);
    return outImage;
}

double bilinear(imgContainer image, int channel, vec2 coords) {
    int topLeftIndex = image.height * image.width * channel + image.width * (int)coords.y + (int)coords.x;
    int bottomLeftIndex = topLeftIndex + image.width;

    // Interpolate along the x-axis
    double xWeight = coords.x - floor(coords.x);
    double interpolateTop = image.imageData[topLeftIndex] + (image.imageData[topLeftIndex + 1] - image.imageData[topLeftIndex]) * xWeight;
    double interpolateBottom = image.imageData[bottomLeftIndex] + (image.imageData[bottomLeftIndex + 1] - image.imageData[bottomLeftIndex]) * xWeight;

    // Interpolate along the y-axis
    double yWeight = coords.y - floor(coords.y);
    double interpolated = interpolateTop + (interpolateBottom - interpolateTop) * yWeight;

    return interpolated;
}