#include <math.h>

#include "circlePatternAlign.h"
#include "interpolate.h"

imgAlignment* circlePatternAlign(imgContainer reference, imgContainer* images, int imagesLen) {
    imgAlignment* outImages = malloc(imagesLen * sizeof(imgAlignment));

    vec2 refCOM = getCentreOfMass(reference);
    for(int i = 0; i < imagesLen; i++) {
        vec2 imgCOM = getCentreOfMass(images[i]);
    }
}

vec2 getCentreOfMass(imgContainer img) {
    vec2 pixelSum = {0.0, 0.0};
    double sumWeights = 0.0;
    for(int y = 0; y < img.height; y++) {
        for(int x = 0; x < img.width; x++) {
            double intensity = img.imageData[y * img.width + x];
            pixelSum.x += x * intensity;
            pixelSum.y += y * intensity;
        }
    }

    pixelSum.x /= sumWeights;
    pixelSum.y /= sumWeights;
    return pixelSum;
}

double sumOfDiffSquares(imgContainer img1, imgContainer img2) {
    double sum = 0.0;
    for(int i = 0; i < img1.width * img1.height; i++) {
        double diff = img1.imageData[i] - img2.imageData[i];
        sum += diff * diff;
    }
}

double getRotation(imgContainer reference, vec2 refCOM, imgContainer img, vec2 imgCOM) {
    const int iterations = 4;
    const int divisions = 16;

    double errors[divisions];
    double minAngle = 0.0;
    double maxAngle = 2.0 * M_PI;

    for(int i = 0; i < iterations; i++) {
        double minError = getRotationError(reference, refCOM, img, imgCOM, minAngle);
        int minErrorIndex = 0;
        double delta = (maxAngle - minAngle) / divisions;

        for(int j = 1; j <= divisions; j++) {
            double error = getRotationError(reference, refCOM, img, imgCOM, minAngle + delta * j);
            if(error < minError) {
                minError = error;
                minErrorIndex = i;
            }
        }

        maxAngle = minAngle + delta * (minErrorIndex + 1);
        minAngle = minAngle + delta * (minErrorIndex - 1);
    }

    return (minAngle + maxAngle) / 2.0;
}

double getRotationError(imgContainer reference, vec2 refCOM, imgContainer img, vec2 imgCOM, double angle) {
    const int windowWidth = 16;

    imgContainer refWindow = bilinearInterpolationWindow(reference, 0.0, refCOM, windowWidth);
    imgContainer imgWindow = bilinearInterpolationWindow(img, angle, imgCOM, windowWidth);

    return sumOfDiffSquares(refWindow, imgWindow);
}