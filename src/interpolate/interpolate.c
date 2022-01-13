#include <stdlib.h>
#include <math.h>

#include "interpolatePrivate.h"

imgContainer bilinearInterpolation(imgAlignment img) {
    imgContainer outImage = alignImage(img, bilinear);
    return outImage;
}

vec2 transform(imgAlignment img, int x, int y) {
    double rotCos = cos(img.angle);
    double rotSin = sin(img.angle);
    return (vec2){rotCos * x - rotSin * y + img.dx,
                    rotSin * x + rotCos * y + img.dy};
}

vec2 untransform(imgAlignment img, int x, int y) {
    double rotCos = cos(img.angle);
    double rotSin = sin(img.angle);
    double xt = x - img.dx;
    double yt = y - img.dy;
    return (vec2){rotCos * xt + rotSin * yt,
                    -rotSin * xt + rotCos * yt};
}

imgContainer alignImage(imgAlignment img, double (*interpolate)(imgContainer img, int channel, vec2 coords)) {
    // Create container for interpolated image
    int imageDataLen = img.image.width * img.image.height * img.image.channels;
    imgContainer stackedImage = img.image;
    stackedImage.imageData = malloc(imageDataLen * sizeof(double));
    vec2 topLeft = untransform(img, 0, 0);
    vec2 diff = subtract(untransform(img, 1, 1), topLeft);
    
    // Get interpolated value for each pixel
    for(int y = 0; y < stackedImage.height; y++) {
        for(int x = 0; x < stackedImage.width; x++) {
            for(int i = 0; i < stackedImage.channels; i++) {
                int dataIndex = stackedImage.width * stackedImage.height * i + stackedImage.width * y + x;
                vec2 coords = untransform(img, x, y);//{topLeft.x + x * diff.x, topLeft.y + y * diff.y};
                double interpolatedValue;
                if(coords.x >= 0 && coords.x <= stackedImage.width - 1 &&
                    coords.y >= 0 && coords.y <= stackedImage.height - 1) {
                    interpolatedValue = interpolate(img.image, i, coords);
                } else {
                    interpolatedValue = 0.0;
                }
                stackedImage.imageData[dataIndex] = interpolatedValue;
            }
        }
    }

    return stackedImage;
}

double bilinear(imgContainer img, int channel, vec2 coords) {
    int topLeftIndex = img.height * img.width * channel + img.width * (int)coords.y + (int)coords.x;
    int bottomLeftIndex = topLeftIndex + img.width;

    // Interpolate along the x-axis
    double xWeight = coords.x - floor(coords.x);
    double interpolateTop = img.imageData[topLeftIndex] + (img.imageData[topLeftIndex + 1] - img.imageData[topLeftIndex]) * xWeight;
    double interpolateBottom = img.imageData[bottomLeftIndex] + (img.imageData[bottomLeftIndex + 1] - img.imageData[bottomLeftIndex]) * xWeight;

    // Interpolate along the y-axis
    double yWeight = coords.y - floor(coords.y);
    double interpolated = interpolateTop + (interpolateBottom - interpolateTop) * yWeight;

    return interpolated;
}