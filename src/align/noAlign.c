#include <stdlib.h>

#include "align.h"

imgAlignment* noAlign(imgContainer reference, imgContainer* images, int imagesLen, int alignChannel) {
    imgAlignment* aligned = malloc(imagesLen * sizeof(imgAlignment));
    for(int i = 0; i < imagesLen; i++) {
        aligned[i] = (imgAlignment){images[i], 0.0, 0.0, 0.0};
    }
    return aligned;
}