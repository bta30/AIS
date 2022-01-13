#ifndef ARGS_PRIVATE_H
#define ARGS_PRIVATE_H

#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "align.h"
#include "errors.h"
#include "interpolate.h"
#include "stack.h"

// String containing output for the help screen
const char* helpOutput =
    "Usage: ais [options]\n"
    "Options:\n"
    "-in [image1] [image2] ...\t\tSpecify input images\n"
    "-stdin\t\tSpecify input images using standard input, one line per file path\n"
    "-out [image]\t\tSpecify output image path (defaults to out.tif)\n"
    "-bias [image]\t\tSpecify master bias image\n"
    "-dark [image]\t\tSpecify master dark image\n"
    "-flat [image]\t\tSpecify master flat image\n"
    "-alignImage [image]\t\tAlign aligment image (defaults to first input file given)\n"
    "-align [method]\t\tSpecify alignment method (noalign (default), starthreshold)\n"
    "-stack [method]\t\tSpecify stacking method (mean (default), meansigmareject)\n"
    "-interpolate [method]\t\tSpecify interpolation method (bilinear (default))\n"
    "-help\t\tPrint this help screen";

// Error strings
// Too many input file arguments passed in
const char* tooManyInputFileArgsError = "Error: Input file arguments (-in, -stdin) passed in at least twice";
// No input files given
const char* noInputFileError = "Error: No input files given";
// Given the same argument at least twice
const char* duplicateError = "Error: Duplicate argument type given, ";
// Insufficient arguments given for a specific option
const char* insufficientArgsError = "Error: Insufficient arguments given with ";
// Too many arguments given for a specific option
const char* tooManyArgsError = "Error: Too many arguments given with ";
// Invalid argument name given
const char* invalidArgNameError = "Error: Invalid argument name ";
// Invalid choice given
const char* invalidChoiceError = "Error: Invalid choice for ";

// Default set of arguments for the program
programArgs defaultArgs = {NULL, 0, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0};
char* defaultOutput = "out.tif";
alignFunc defaultAlign = noAlign;
stackFunc defaultStack = meanStack;
interpolationFunc defaultInterpolate = bilinearInterpolation;

// Add a specified argument to a programArgs instance
// Return values: 0 (success), 1 (success, early exit), -1 (unsuccessful, early exit), 2 (read from file names from stdin afterwards)
int processCurrArg(int argc, char** argv, programArgs* args, int argIndex);

int processIn(int argc, char** argv, programArgs* args, int argIndex);  // Process a -in argument
int processStdin(programArgs* args);  // Process a -stdin argument
int processFileNameArg(int argc, char** argv, char** fileNameDest, int argIndex);  // Process an argument that requires a single file name
int processAlign(int argc, char** argv, programArgs* args, int argIndex);  // Process an alignment method argument
int processStack(int argc, char** argv, programArgs* args, int argIndex);  // Process a stacking method option
int processInterpolate(int argc, char** argv, programArgs* args, int argIndex);  // Process an interpolation method option

int readFirstFileNameStdin(programArgs* args, int bufferSize);  // Read the first file name into args, creating the buffer
int readRemainingFileNamesStdin(programArgs* args, int bufferSize);  // Read remaining file names from stdin to args
char* readLineStdin();  // Returns a full line from stdin

alignFunc chooseAlign(char* alignName);  // Choose an alignment function given its name
stackFunc chooseStack(char* stackName);  // Choose a stacking function given its name
interpolationFunc chooseInterpolate(char* stackName);  // Choose an interpolation function given its name

int ensureOneArg(int argc, char** argv, int argIndex);  // Check that exactly one argument has been given for an argument
int countArgs(int argc, char** argv, int argIndex);  // Count the number of arguments given for an argument

int checkFirstArgFormat(int argc, char** argv);  // Check that the first argument exists and starts with '-'
int iterateArgs(int argc, char** argv, programArgs* args);  // Process each of the given arguments iteratively

void addDefaultValues(programArgs* args);  // Adds default values to arguments whose values have not been set

#endif