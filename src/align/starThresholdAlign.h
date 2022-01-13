#ifndef STARTHRESHOLDALIGN_H
#define STARTHRESHOLDALIGN_H

#include "align.h"
#include "vec.h"

const int minStarArea = 32;
const int comparedTris = 32;
const int maxCompareTris = 1000000;
const int chosenTris = 16;
const double maxError = 0.1;
const int topChooseStars = 5;

typedef struct {
    vec2* coords;
    int len;
} starsList;

// Stores the vertices and lengths of a triangle in clockwise order
typedef struct {
    vec2 coords[3];
    double lengths[3];
} starTriangle;

typedef struct {
    starTriangle* tris;
    int length;
} starTriangleList;

typedef struct {
    starTriangle refTri;
    starTriangle imageTri;
    double error;
} starTriangleMatch;

double getStarThreshold(imgContainer image);  // Determine threshold for star brightness
starsList getStars(imgContainer image, double threshold);  // Generate list of stars present in an image
starTriangleList getTriangles(starsList stars);  // Get list of star triangles given list of stars in an image

int getStar(int* image, int x, int y, int width, int height, vec2* centre, int* pixelQueue); // Get the centre of the star starting at a particular coordinate given, erasing it, returns 0 if the star given is invalid

alignInfo getAlignment(starTriangleList refTris, starsList imageStar);  // Gets alignment between a starTriangleList and a starsList
starTriangleMatch* getMatchingTriangles(starTriangleList refTris, starsList imageStars, int length);  // Gets length matching pairs triangles of stars between a starsList and a starTriangleList
starTriangleMatch* getLowestErrorTriangles(starTriangleMatch* matches, int matchesLength, int length);  // Gets a list of the matches with the length smallest errors
alignInfo getMatchAlignment(starTriangleMatch match);  // Gets the alignment from a match of triangles
alignInfo getMeanAlignment(alignInfo* alignments, int length);  // Gets the mean alignment given a list of alignments

starTriangle rotateStartSmallestLength(starTriangle tri);  // Rotates vertices in a starTriangle so that the smallest length is first
double getSideRotation(vec2 side1, vec2 side2, double lengthSquared);
vec2 getTranslation(starTriangleMatch match, double angle);

starTriangle getRandomTriangle(starsList stars);  // Gets a random triangle from a list of stars
starTriangle getClosestTriangle(starTriangle tri, starTriangleList tris);  // Gets the closest triangle in a starTriangleList to a given starTriangle
double triangleError(starTriangle tri1, starTriangle tri2);  // Gets the error between the lengths of two triangles

#endif