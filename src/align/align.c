#include "alignPrivate.h"

imgAlignment noAlign(imgContainer reference, imgContainer image) {
    return (imgAlignment) {image, 0, 0, 0.0};
}