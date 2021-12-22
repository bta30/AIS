#include "args.h"

int main(int argc, char** argv) {
    programArgs args;

    int err;
    if(err = processArgs(argc, argv, &args)) {
        return err;
    }

    return 0;
}