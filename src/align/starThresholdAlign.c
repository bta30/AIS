#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "starThresholdAlign.h"

imgAlignment* starThresholdAlign(imgContainer reference, imgContainer* images, int imagesLen) {
    imgAlignment* outImages = malloc(imagesLen * sizeof(imgAlignment));
    srand(time(NULL));
    
    double threshold = getStarThreshold(reference);
    starTriangleList refTris = getTriangles(getStars(reference, threshold));

    for(int i = 0; i < imagesLen; i++) {
        starsList currStars = getStars(images[i], threshold);
        alignInfo alignment = getAlignment(refTris, currStars);
        outImages[i] = (imgAlignment){images[i], alignment.dx, alignment.dy, alignment.angle};
    }

    return outImages;
}

double getStarThreshold(imgContainer image) {
    // Get mean and standard deviation
    int pixelsNum = image.width * image.height;
    double sumX = 0.0;
    double sumXSquared = 0.0;
    for(int i = 0; i < pixelsNum; i++) {
        sumX += image.imageData[i];
        sumXSquared += image.imageData[i] * image.imageData[i];
    }
    double mean = sumX / pixelsNum;
    double sd = sqrt(sumXSquared / pixelsNum - mean * mean);

    return mean + sd;
}

starsList getStars(imgContainer image, double threshold) {
    // Generate binary thresholded image
    int* thresholdImage = malloc(image.width * image.height * sizeof(int));
    for(int i = 0; i < image.height * image.width; i++) {
        thresholdImage[i] = image.imageData[i] > threshold;
    }

    // Retrieve each star in the image
    int bufferSize = comparedTris;
    starsList stars = {malloc(bufferSize * sizeof(vec2)), 0};
    for(int y = 0; y < image.height; y++) {
        for(int x = 0; x < image.width; x++) {
            if(thresholdImage[y * image.width + x]) {
                if(stars.len == bufferSize) {
                    bufferSize *= 2;
                    stars.coords = realloc(stars.coords, bufferSize);
                }

                if(getStar(thresholdImage, x, y, image.width, image.height, stars.coords + stars.len)) {
                    stars.len++;
                }
            }
        }
    }

    return stars;
}

int getStar(int* image, int x, int y, int width, int height, vec2* centre) {
    // Initialise the queue of pixels in the star
    int bufferSize = 1024;
    int* pixelQueue = malloc(bufferSize * sizeof(int));
    pixelQueue[0] = x;
    pixelQueue[1] = y;
    image[y * width + x] = 0;
    int queueHead = 0, queueTail = 2;

    // Go through each pixel in the star, summing all x and y coords
    int sumX = 0, sumY = 0;
    int pixelCount = 1;
    while(queueHead != queueTail) {
        // Add the current coordinates to the running sum
        sumX += pixelQueue[queueHead];
        sumY += pixelQueue[queueHead + 1];

        // Add the neighbours to queue
        for(int dy = -1; dy <= 1; dy++) {
            for(int dx = -1; dx <= 1; dx++) {
                int newX = pixelQueue[queueHead] + dx;
                int newY = pixelQueue[queueHead + 1] + dy;

                if(newY >= 0 && newY < height &&
                    newX >= 0 && newX < width &&
                    image[newY * width + x]) {
                        pixelQueue[queueTail] = newX;
                        pixelQueue[queueTail + 1] = newY;
                        queueTail += 2;

                        if(queueTail == queueHead) {
                            bufferSize *= 2;
                            pixelQueue = realloc(pixelQueue, bufferSize);
                        }

                        if(queueTail >= bufferSize) {
                            queueTail -= bufferSize;
                        }
                    }
            }
        }

        queueHead += 2;
        pixelCount++;
    }

    // Set the centre of the star of it is big enough
    if(pixelCount < minStarArea) {
        return 1;
    } else {
        *centre = (vec2){(double)sumX / pixelCount, (double)sumY / pixelCount};
        return 0;
    }
}

starTriangleList getTriangles(starsList stars) {
    // Allocate list of star triangles
    int trianglesLen = stars.len * (stars.len -  1) * (stars.len - 2);
    starTriangleList tris = {malloc(trianglesLen * sizeof(starTriangle)), trianglesLen};

    // Go through each triplet of stars to form a triangle
    int index = 0;
    for(int i = 0; i < stars.len; i++) {
        for(int j = i + 1; j < stars.len; j++) {
            for(int k = j + 1; k < stars.len; k++) {
                // Generate the curren triangle
                starTriangle currTri = {{stars.coords[i], stars.coords[j], stars.coords[k]},
                                        {diffSquared(stars.coords[i], stars.coords[j]),
                                            diffSquared(stars.coords[j], stars.coords[k]),
                                            diffSquared(stars.coords[k], stars.coords[i])}};

                // Set the lengths squares of the current triangle
                tris.tris[index++] = currTri;
            }
        }
    }

    return tris;
}

alignInfo getAlignment(starTriangleList refTris, starsList imgStars) {
    starTriangleMatch* matches = getMatchingTriangles(refTris, imgStars, comparedTris);
    if(matches == NULL) {
        free(matches);
        return (alignInfo){0.0, 0.0, 0.0};
    }

    starTriangleMatch* bestTris = getLowestErrorTriangles(matches, comparedTris, chosenTris);
    
    alignInfo alignments[chosenTris];
    for(int i = 0; i < chosenTris; i++) {
        alignments[i] = getMatchAlignment(bestTris[i]);
    }
    
    free(matches);
    free(bestTris);

    return getMeanAlignment(alignments, chosenTris);
}

starTriangleMatch* getMatchingTriangles(starTriangleList refTris, starsList imgStars, int length) {
    starTriangleMatch* matches = malloc(length * sizeof(starTriangleMatch));

    int i = 0;
    int totalCompared = 0;
    while(i < length && totalCompared < maxCompareTris) {
        starTriangle imgTri = getRandomTriangle(imgStars);
        starTriangle closestTri = getClosestTriangle(imgTri, refTris);
        double error = triangleError(imgTri, closestTri);

        if(error < maxError) {
            matches[i++] = (starTriangleMatch) {closestTri, imgTri, error};
        }

        totalCompared++;
    }

    if(totalCompared == maxCompareTris) {
        free(matches);
    }

    return totalCompared == maxCompareTris ? NULL : matches;
}

starTriangleMatch* getLowestErrorTriangles(starTriangleMatch* matches, int matchesLength, int length) {
    // Fill up buffer of lowest errors with start of matches list
    starTriangleMatch* lowestTris = malloc(length * sizeof(starTriangleMatch));
    double maxError = matches[0].error;
    for(int i = 0; i < length; i++) {
        lowestTris[i] = matches[i];
        if(lowestTris[i].error > maxError) {
            maxError = lowestTris[i].error;
        }
    }

    // For each element in the matches list, put it into the lowestTris list if it has a low enough error
    for(int i = length; i < matchesLength; i++) {
        if(matches[i].error > lowestTris[i].error) {
            double newMaxError = matches[i].error;
            for(int j = 0; j < length; j++) {
                if(lowestTris[j].error = maxError) {
                    lowestTris[j] = matches[i];
                }
                else if(lowestTris[j].error > newMaxError) {
                    newMaxError = lowestTris[j].error;
                }
            }
            maxError = newMaxError;
        }
    }

    return lowestTris;
}

alignInfo getMatchAlignment(starTriangleMatch match) {
    // Puts the vertices of the triangles into the correct order
    match.refTri = rotateStartSmallestLength(match.refTri);
    match.imgTri = rotateStartSmallestLength(match.imgTri);
    
    vec2 translation = subtract(match.refTri.coords[0], match.imgTri.coords[0]);

    vec2 refSide = subtract(match.refTri.coords[1], match.refTri.coords[0]);
    vec2 imgSide = subtract(match.imgTri.coords[1], match.imgTri.coords[0]);
    double angle = getSideRotation(imgSide, refSide, match.refTri.lengthsSquares[0]);

    return (alignInfo){translation.x, translation.y, angle};
}

alignInfo getMeanAlignment(alignInfo* alignments, int length) {
    alignInfo meanAlignment = {0.0, 0.0, 0.0};
    for(int i = 0; i < length; i++) {
        meanAlignment.dx += alignments[i].dx;
        meanAlignment.dy += alignments[i].dy;
        meanAlignment.angle += alignments[i].angle;
    }

    meanAlignment.dx /= length;
    meanAlignment.dy /= length;
    meanAlignment.angle /= length;
}

starTriangle rotateStartSmallestLength(starTriangle tri) {
    // Identify index of smallest length
    int smallestLength = (tri.lengthsSquares[0] < tri.lengthsSquares[1]) ? 0 : 1;
    if(tri.lengthsSquares[2] < tri.lengthsSquares[smallestLength]) {
        smallestLength = 2;
    }

    // Rotate vertices as required
    switch(smallestLength) {
        case 0:
            return tri;
        case 1:
            return (starTriangle){{tri.coords[1], tri.coords[2], tri.coords[0]},
                {tri.lengthsSquares[1], tri.lengthsSquares[2], tri.lengthsSquares[0]}};
        case 2:
            return (starTriangle){{tri.coords[2], tri.coords[0], tri.coords[1]},
                {tri.lengthsSquares[2], tri.lengthsSquares[0], tri.lengthsSquares[1]}};
    }
}

double getSideRotation(vec2 side1, vec2 side2, double lengthSquared) {
    return atan2(side1.x * side2.x + side1.y * side2.y,
                    side1.x * side2.y - side1.y * side2.x);
}

starTriangle getRandomTriangle(starsList stars) {
    // Choose random indices of 3 unique vertices
    int vertices[3] = {rand() % stars.len, rand() % (stars.len - 1), rand() % (stars.len - 2)};
    if(vertices[1] > vertices[0]) {
        vertices[1]++;
    }
    if(vertices[2] >= vertices[0]) {
        vertices[2]++;
    }
    if(vertices[2] >= vertices[1]) {
        vertices[2]++;
    }

    // Get the triangle corresponding to the chosen vertices
    starTriangle tri = {{stars.coords[vertices[0]], stars.coords[vertices[1]], stars.coords[vertices[2]]}, {}};
    tri.lengthsSquares[0] = diffSquared(tri.coords[1], tri.coords[0]);
    tri.lengthsSquares[1] = diffSquared(tri.coords[2], tri.coords[1]);
    tri.lengthsSquares[2] = diffSquared(tri.coords[0], tri.coords[2]);

    return tri;
}

starTriangle getClosestTriangle(starTriangle tri, starTriangleList tris) {
    starTriangle closestTri = tris.tris[0];
    double triError = triangleError(tri, closestTri);

    for(int i = 1; i < tris.length; i++) {
        double newTriError = triangleError(tri, tris.tris[i]);
        if(newTriError < triError) {
            closestTri = tris.tris[i];
            triError = newTriError;
        }
    }

    return closestTri;
}

double triangleError(starTriangle tri1, starTriangle tri2) {
    // Put the vertices of the triangles in the correct order
    tri1 = rotateStartSmallestLength(tri1);
    tri2 = rotateStartSmallestLength(tri2);

    // Calculate error
    double diffs[3] = {tri1.lengthsSquares[0] - tri2.lengthsSquares[0],
                        tri1.lengthsSquares[1] - tri2.lengthsSquares[2],
                        tri1.lengthsSquares[2] - tri2.lengthsSquares[2]};
    double error = diffs[0] * diffs[0] + diffs[1] * diffs[1] + diffs[3] * diffs[3];

    return error;
}