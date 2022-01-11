#include <stdlib.h>
#include <tiffio.h>

#include "imgioPrivate.h"

imgContainer loadImage(const char* filepath) {
    TIFF* imageFile = TIFFOpen(filepath, "r");
    imgContainer img;

    if(imageFile) {
        // Retrieve metadata info from image
        TIFFGetField(imageFile, TIFFTAG_IMAGEWIDTH, &img.width);
        TIFFGetField(imageFile, TIFFTAG_IMAGELENGTH, &img.height);
        TIFFGetField(imageFile, TIFFTAG_SAMPLESPERPIXEL, &img.channels);

        img.imageData = malloc(img.width * img.height * img.channels * sizeof(double));
        if(img.imageData == NULL) {
            fprintf(stderr, "%s\n", memoryAllocationError);
            TIFFClose(imageFile);
            return (imgContainer){NULL, 0, 0, 0};
        }

        uint32_t* raster = _TIFFmalloc(img.width * img.height * sizeof(uint32_t));

        if(raster != NULL) {
            if(TIFFReadRGBAImage(imageFile, img.width, img.height, raster, 0)) {
                for(int i = 0; i < img.width * img.height; i++) {
                    img.imageData[i] = (double)TIFFGetR(raster[i])/(1<<8);
                    img.imageData[img.width * img.height + i] = (double)TIFFGetG(raster[i])/(1<<8);
                    img.imageData[2 * img.width * img.height + i] = (double)TIFFGetB(raster[i])/(1<<8);
                }
            } else {
                fprintf(stderr, "%s%s\n", unableReadFileError, filepath);
                _TIFFfree(raster);
                free(img.imageData);
                TIFFClose(imageFile);
                return (imgContainer){NULL, 0, 0, 0};
            }
            _TIFFfree(raster);
        } else {
            fprintf(stderr, "%s\n", memoryAllocationError);
            free(img.imageData);
            TIFFClose(imageFile);
            return (imgContainer){NULL, 0, 0, 0};
        }

        TIFFClose(imageFile);

    } else {
        fprintf(stderr, "%s%s\n", unableOpenFileError, filepath);
        img = (imgContainer){NULL, 0, 0, 0};
    }

    return img;
}

int exportImage(imgContainer image,  const char* filepath) {
    TIFF* imageFile = TIFFOpen(filepath, "w");

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
                //printf("%d %d %dS\n", i, j, k);
                buffer[j * image.channels + k] = (uint16_t)(image.imageData[k * image.height * image.width + i * image.width  + j] * (1<<16));
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