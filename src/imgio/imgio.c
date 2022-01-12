#include <stdlib.h>
#include <tiffio.h>

#include "imgioPrivate.h"

imgContainer loadImage(char* filePath) {
    TIFF* imageFile = TIFFOpen(filePath, "r");
    imgContainer img;

    if(imageFile) {
        // Retrieve metadata info from image
        TIFFGetField(imageFile, TIFFTAG_IMAGEWIDTH, &img.width);
        TIFFGetField(imageFile, TIFFTAG_IMAGELENGTH, &img.height);
        img.channels = 3;
        TIFFGetField(imageFile, TIFFTAG_SAMPLESPERPIXEL, &img.channels);
        img.filePath = filePath;

        img.imageData = malloc(img.width * img.height * img.channels * sizeof(double));
        if(img.imageData == NULL) {
            fprintf(stderr, "%s\n", memoryAllocationError);
            TIFFClose(imageFile);
            return (imgContainer){NULL, 0, 0, 0, NULL};
        }

        uint32_t* raster = _TIFFmalloc(img.width * img.height * sizeof(uint32_t));
        if(raster != NULL) {
            if(TIFFReadRGBAImage(imageFile, img.width, img.height, raster, 0)) {
                for(int y = 0; y < img.height; y++) {
                    for(int x = 0; x < img.width; x++) {
                        int tiffIndex = (img.height - y - 1) * img.width + x;
                        int imageDataIndex = y * img.width + x;
                        img.imageData[imageDataIndex] = (double)TIFFGetR(raster[tiffIndex])/(1<<8);
                        img.imageData[img.width * img.height + imageDataIndex] = (double)TIFFGetG(raster[tiffIndex])/(1<<8);
                        img.imageData[2 * img.width * img.height + imageDataIndex] = (double)TIFFGetB(raster[tiffIndex])/(1<<8);
                    }
                }
            } else {
                fprintf(stderr, "%s%s\n", unableReadFileError, filePath);
                _TIFFfree(raster);
                free(img.imageData);
                TIFFClose(imageFile);
                return (imgContainer){NULL, 0, 0, 0, NULL};
            }
            _TIFFfree(raster);
        } else {
            fprintf(stderr, "%s\n", memoryAllocationError);
            free(img.imageData);
            TIFFClose(imageFile);
            return (imgContainer){NULL, 0, 0, 0, NULL};
        }
        TIFFClose(imageFile);
    } else {
        fprintf(stderr, "%s%s\n", unableOpenFileError, filePath);
        img = (imgContainer){NULL, 0, 0, 0, NULL};
    }

    return img;
}

imgContainer* loadImages(char** fileNames, int fileNamesLen) {
    // Allocate space for images
    imgContainer* images = malloc(fileNamesLen * sizeof(imgContainer));
    if(images == NULL) {
        fprintf(stderr, "%s\n", memoryAllocationError);
        return NULL;
    }

    // Allocate each image
    for(int i = 0; i < fileNamesLen; i++) {
        images[i] = loadImage(fileNames[i]);
        if(images[i].imageData == NULL) {
            free(images);
            return NULL;
        }
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