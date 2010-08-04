#ifndef STEREO_H
#define STEREO_H

#include "pattern.h"

typedef struct {
    /* The actual image data */
    StereoPattern *image;

    /* The pattern used */
    StereoPattern *pattern;
} StereoImage;

/*
 * Creates a stereogram image with the specified dimensions.
 *
 * pattern is used as the background pattern. This function will fail if it is
 * not specified.
 */
StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern);

/*
 * Applies a Z-buffer to the stereo image, creating an actual stereogram.
 *
 * The result is non-zero upon success, or 0 if the dimensions of the buffer do
 * not match the dimensions of the stereo image.
 */
int
stereo_image_apply(StereoImage *image, StereoPattern *zbuffer);

#endif
