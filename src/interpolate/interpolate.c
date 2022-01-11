#include <stdlib.h>
#include <math.h>

#include "interpolatePrivate.h"

imgContainer bilinearInterpolation(imgAlignment img) {
    return alignImage(img, bilinear);
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
    
    // Get interpolated value for each pixel
    for(int y = 0; y < stackedImage.height; y++) {
        for(int x = 0; x < stackedImage.width; x++) {
            for(int i = 0; i < stackedImage.channels; i++) {
                int dataIndex = stackedImage.width * stackedImage.height * i + stackedImage.width * y + x;
                vec2 coords = untransform(img, x, y);
                double interpolatedValue;
                if(coords.x >= 0 && coords.x <= stackedImage.width - 1 &&
                    coords.y >= 0 && coords.y <= stackedImage.height - 1) {
                    interpolatedValue = interpolate(img.image, i, coords);
                } else {
                    interpolatedValue = NAN;
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

imgContainer bilinearInterpolationWindow(imgContainer img, double angle, vec2 centre, int windowWidth) {
    imgContainer window = {malloc(windowWidth * windowWidth * sizeof(double)), windowWidth, windowWidth, 1};
    
    vec2 deltaX = {cos(angle), sin(angle)};
    vec2 deltaY = {-sin(angle), cos(angle)};

    vec2 topLeft = centre;
    topLeft.x -= windowWidth;
    topLeft.y -= windowWidth;

    for(int y = 0; y < windowWidth; y++) {
        for(int x = 0; x < windowWidth; x++) {
            vec2 currCoords = topLeft;
            currCoords.x += deltaX.x * x + deltaY.x * y;
            currCoords.y += deltaX.y * x + deltaY.y * y;

            window.imageData[y * windowWidth + x] = bilinear(img, 0, currCoords);
        }
    }

    return window;
}