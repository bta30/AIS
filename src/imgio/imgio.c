#include <stdlib.h>
#include <tiffio.h>

#include "imgioPrivate.h"

imgContainer loadImage(char* filePath) {
    TIFF* imageFile = TIFFOpen(filePath, "r");
    imgContainer image;

    if(imageFile) {
        // Retrieve metadata info from image
        TIFFGetField(imageFile, TIFFTAG_IMAGEWIDTH, &image.width);
        TIFFGetField(imageFile, TIFFTAG_IMAGELENGTH, &image.height);
        image.channels = 3;
        TIFFGetField(imageFile, TIFFTAG_SAMPLESPERPIXEL, &image.channels);
        image.filePath = filePath;
        image.toDeleteFilePath = 0;

        image.imageData = malloc(image.width * image.height * image.channels * sizeof(double));
        if(image.imageData == NULL) {
            fprintf(stderr, "%s\n", memoryAllocationError);
            TIFFClose(imageFile);
            return (imgContainer){NULL, 0, 0, 0, NULL, 0};
        }

        uint32_t* raster = _TIFFmalloc(image.width * image.height * sizeof(uint32_t));
        if(raster != NULL) {
            if(TIFFReadRGBAImage(imageFile, image.width, image.height, raster, 0)) {
                for(int y = 0; y < image.height; y++) {
                    for(int x = 0; x < image.width; x++) {
                        int tiffIndex = (image.height - y - 1) * image.width + x;
                        int imageDataIndex = y * image.width + x;
                        image.imageData[imageDataIndex] = (double)TIFFGetR(raster[tiffIndex])/(1<<8);
                        image.imageData[image.width * image.height + imageDataIndex] = (double)TIFFGetG(raster[tiffIndex])/(1<<8);
                        image.imageData[2 * image.width * image.height + imageDataIndex] = (double)TIFFGetB(raster[tiffIndex])/(1<<8);
                    }
                }
            } else {
                fprintf(stderr, "%s%s\n", unableReadFileError, filePath);
                _TIFFfree(raster);
                free(image.imageData);
                TIFFClose(imageFile);
                return (imgContainer){NULL, 0, 0, 0, NULL, 0};
            }
            _TIFFfree(raster);
        } else {
            fprintf(stderr, "%s\n", memoryAllocationError);
            free(image.imageData);
            TIFFClose(imageFile);
            return (imgContainer){NULL, 0, 0, 0, NULL, 0};
        }
        TIFFClose(imageFile);
    } else {
        fprintf(stderr, "%s%s\n", unableOpenFileError, filePath);
        image = (imgContainer){NULL, 0, 0, 0, NULL, 0};
    }

    return image;
}

imgContainer* loadImages(char** fileNames, int fileNamesLen) {
    // Allocate space for images
    imgContainer* images = malloc(fileNamesLen * sizeof(imgContainer));
    if(images == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return NULL;
    }

    // Set file name for each image
    for(int i = 0; i < fileNamesLen; i++) {
        images[i] = (imgContainer){NULL, 0, 0, 0, fileNames[i], 0};
    }

    return images;
}

int exportImage(imgContainer image,  const char* filePath) {
    TIFF* imageFile = TIFFOpen(filePath, "w");

    // Set metadata of image
    TIFFSetField(imageFile, TIFFTAG_IMAGEWIDTH, image.width);
    TIFFSetField(imageFile, TIFFTAG_IMAGELENGTH, image.height);
    TIFFSetField(imageFile, TIFFTAG_SAMPLESPERPIXEL, image.channels);
    TIFFSetField(imageFile, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(imageFile, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(imageFile, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(imageFile, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    tsize_t linebytes = image.channels * image.width;
    uint16_t* buffer = (uint16_t*)_TIFFmalloc(TIFFScanlineSize(imageFile));
    TIFFSetField(imageFile, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(imageFile, linebytes));

    for(int i = 0; i < image.height; i++) {
        for(int j = 0; j < image.width; j++) {
            for(int k = 0; k < image.channels; k++) {
                double scaledValue = image.imageData[k * image.height * image.width + i * image.width  + j] * (1<<16);
                if(scaledValue < 0.0) {
                    scaledValue = 0.0;
                } else if(scaledValue > (1<<16) - 1) {
                    scaledValue = (1<<16) - 1;
                }
                buffer[j * image.channels + k] = (uint16_t)scaledValue;
            }
        }

        if(TIFFWriteScanline(imageFile, buffer, i, 0) < 0) {
            break;
        }
    }

    TIFFClose(imageFile);
    if(buffer) {
        _TIFFfree(buffer);
    }
}