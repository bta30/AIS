#include <stdio.h>

#include "argsPrivate.h"

int processArgs(int argc, char** argv, programArgs* args) {
    if(checkFirstArgFormat(argc, argv)) {
        return 0;
    }

    *args = defaultArgs;
    int err;
    if(err = iterateArgs(argc, argv, args)) {
        return err == 1 ? 0 : err;
    }

    if(args->fileNames == NULL) {
        fprintf(stderr, "%s\n", noInputFileError);
    }

    return 0;
}

int processCurrArg(int argc, char** argv, programArgs* args, int argIndex) {
    char* argName = argv[argIndex];
    if(strcmp(argName, "-in") == 0) {
        return processIn(argc, argv, args, argIndex);
    } else if(strcmp(argName, "-stdin") == 0) {
        return 2;
    } else if(strcmp(argName, "-bias") == 0) {
        return processFileNameArg(argc, argv, &(args->bias), argIndex);
    } else if(strcmp(argName, "-dark") == 0) {
        return processFileNameArg(argc, argv, &(args->dark), argIndex);
    } else if(strcmp(argName, "-flat") == 0) {
        return processFileNameArg(argc, argv, &(args->flat), argIndex);
    } else if(strcmp(argName, "-alignImage") == 0) {
        return processFileNameArg(argc, argv, &(args->alignImage), argIndex);
    } else if(strcmp(argName, "-align") == 0) {
        return processAlign(argc, argv, args, argIndex);
    } else if(strcmp(argName, "-stack") == 0) {
        return processAlign(argc, argv, args, argIndex);
    } else if(strcmp(argName, "-help") == 0) {
        printf("%s\n", helpOutput);
        return 1;
    } else {
        fprintf(stderr, "%s%s\n", invalidArgNameError, argName);
        return 1;
    }
}

int processIn(int argc, char** argv, programArgs* args, int argIndex) {
    // Check if input files have already been set
    if(args->fileNames != NULL) {
        fprintf(stderr, "%s\n", tooManyInputFileArgsError);
        return 1;
    }

    // Check that more than zero input files have been given
    if(countArgs(argc, argv, argIndex) == 0) {
        fprintf(stderr, "%s%s\n", insufficientArgsError, "-in");
    }

    // Set the input file fields in args to the given values
    args->fileNames = argv - argIndex + 1;
    args->fileNamesLen = countArgs(argc, argv, argIndex);
    args->deleteFileNames = 0;
    return 0;
}

int processStdin(programArgs* args) {
    // Check if input files have already been set
    if(args -> fileNames != NULL) {
        fprintf(stderr, "%s\n", tooManyInputFileArgsError);
        return 1;
    }

    // Read in all file names
    int err, bufferSize = 256;
    if((err = readFirstFileNameStdin(args, bufferSize)) ||
        (err = readRemainingFileNamesStdin(args, bufferSize))) {
        return err;
    }

    // Reallocate buffer file name buffer size to exactly that needed
    args->fileNames = realloc(args->fileNames, args->fileNamesLen * sizeof(char*));
    if(args->fileNames == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return -1;
    }

    return 0;
}

int processFileNameArg(int argc, char** argv, char** fileNameDest, int argIndex) {
    int err;
    if(err = ensureOneArg(argc, argv, argIndex)) {
        return err;
    }

    // Check the destination is not already filled
    if(*fileNameDest != NULL) {
        fprintf(stderr, "%s%s\n", duplicateError, argv[argIndex]);
        return 1;
    }

    // Write the file name argument into the given destination
    *fileNameDest = argv[argIndex + 1];
    return 0;
}

int processAlign(int argc, char** argv, programArgs* args, int argIndex) {
    int err;
    if(err = ensureOneArg(argc, argv, argIndex)) {
        return err;
    }
    
    if(args->align != NULL) {
        fprintf(stderr, "%s%s\n", duplicateError, argv[argIndex]);
        return 1;
    }

    args->align = chooseAlign(argv[argIndex + 1]);
    if(args->align == NULL) {
        fprintf(stderr, "%s%s, %s\n", invalidChoiceError, argv[argIndex], argv[argIndex + 1]);
        return 1;
    }

    return 0;
}

alignFunc chooseAlign(char* alignName) {
    if(strcmp(alignName, "noalign") == 0) {
        return noAlign;
    } else {
        return NULL;
    }
}

int processStack(int argc, char** argv, programArgs* args, int argIndex) {
    int err;
    if(err = ensureOneArg(argc, argv, argIndex)) {
        return err;
    }

    if(args-> stack != NULL) {
        fprintf(stderr, "%s%s\n", duplicateError, argv[argIndex]);
        return 1;
    }

    args->stack = chooseStack(argv[argIndex + 1]);
    if(args->stack == NULL) {
        fprintf(stderr, "%s%s, %s\n", invalidChoiceError, argv[argIndex], argv[argIndex + 1]);
        return 1;
    }

    return 0;
}

stackFunc chooseStack(char* stackName) {
    if(strcmp(stackName, "mean") == 0) {
        return meanStack;
    } else {
        return NULL;
    }
}

int readFirstFileNameStdin(programArgs* args, int bufferSize) {
    // Check that more than zero input files have been given
    char* firstLine = readLineStdin();
    if(firstLine[0] == '\n') {
        fprintf(stderr, "%s\n", noInputFileError);
        return 1;
    }

    // Creates the buffer and writes the first file name to it
    args->fileNames = malloc(bufferSize * sizeof(char*));
    if(args->fileNames == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return -1;
    }
    args->fileNames[0] = firstLine;
    args->fileNamesLen = 1;

    return 0;
}

int readRemainingFileNamesStdin(programArgs* args, int bufferSize) {
    // Read the remaining file names into args
    char* currLine;
    while((currLine = readLineStdin()) != NULL && currLine[0] != '\n') {
        // Doubles the file name buffer's size if it becomes full
        if(args->fileNamesLen == bufferSize) {
            bufferSize *= 2;
            args->fileNames = realloc(args->fileNames, bufferSize * sizeof(char*));
            if(args->fileNames == NULL) {
                fprintf(stderr, "%s\n", memoryAllocationError);
                return -1;
            }
        }

        // Adds the current file name to args
        args->fileNames[args->fileNamesLen] = currLine;
        args->fileNamesLen++;
    }

    if(currLine == NULL) {
        return -1;
    }

    free(currLine);
    return 0;
}

char* readLineStdin() {
    int bufferSize = 4;
    char* inputBuffer = malloc(bufferSize * sizeof(char));
    if(inputBuffer == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return NULL;
    }
    int bufferOffset = 0;
    char* lineEndPoint = NULL;
    int lineLength = 0;

    // Read data into buffer, increasing its size, until the whole line has been read
    while(lineEndPoint == NULL) {
        // If EOF, end early
        if(fgets(inputBuffer + bufferOffset, bufferSize - bufferOffset, stdin) == NULL) {
            lineLength = bufferOffset + strlen(inputBuffer + bufferOffset) + 1;
            break;
        }

        char* lineEndPoint = strchr(inputBuffer + bufferOffset, '\n');
        if(lineEndPoint == NULL) {
            // Increase buffer size if not reached end of line
            bufferOffset = bufferSize - 1;
            bufferSize *= 2;
            inputBuffer = realloc(inputBuffer, bufferSize);
            if(inputBuffer == NULL) {
                fprintf(stderr, "%s\n", memoryAllocationError);
            }
        } else {
            // Set line length if reached end of line
            lineLength = lineEndPoint - inputBuffer;
        }
    }

    return inputBuffer;
}

int ensureOneArg(int argc, char** argv, int argIndex) {
    int argCount = countArgs(argc, argv, argIndex);
    if(argCount == 0) {
        fprintf(stderr, "%s%s\n", insufficientArgsError, argv[argIndex]);
        return 1; 
    } else if(argCount > 1) {
        fprintf(stderr, "%s%s\n", tooManyArgsError, argv[argIndex]);
        return 1;
    }

    return 0;
}

int countArgs(int argc, char** argv, int argIndex) {
    int currIndex = argIndex + 1;

    while(currIndex != argc && argv[currIndex][0] != '-') {
        currIndex++;
    }

    return currIndex - argIndex - 1;
}

int checkFirstArgFormat(int argc, char** argv) {
    // Print help screen if no arguments given
    if(argc == 1) {
        printf("%s\n", helpOutput);
        return 1;
    }

    // Check that the first argument given is an option
    if(argv[1][0] != '-') {
        fprintf(stderr, "%s%s\n", invalidArgNameError, argv[1]);
        return 1;
    }

    return 0;
}

int iterateArgs(int argc, char** argv, programArgs* args) {
    // Go through each argument iteratively
    int readStdin = 0;
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            int err = processCurrArg(argc, argv, args, i);
            if(err == 2) {
                readStdin = 1;
            } else if(err != 0) {
                return err;
            }
        }
    }

    // Read input files from stdin if required
    int err;
    if(readStdin && (err = processStdin(args))) {
        return err;
    }

    return 0;
}
