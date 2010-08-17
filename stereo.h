#ifndef STEREO_H
#define STEREO_H

#include "pattern.h"
#include "zbuffer.h"

typedef struct {
    /* The actual image data */
    StereoPattern *image;

    /* The pattern used */
    StereoPattern *pattern;

    /* The offsets applied to values in the z-buffer */
    int offsets[256];
} StereoImage;

/**
 * Creates a stereogram image with the specified dimensions.
 *
 * pattern is used as the background pattern. This function will fail if it is
 * not specified.
 *
 * depth is the multiplicator for offsets.
 */
StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern, double depth);

/*
 * Frees a stereo image and its pattern.
 */
void
stereo_image_free(StereoImage *image);

/**
 * Applies a z-buffer to the stereo image, creating an actual stereogram.
 *
 * Only the channel channel is used, and only the lines from start to end are
 * modified.
 *
 * The result is non-zero upon success, or 0 if the dimensions of the buffer do
 * not match the dimensions of the stereo image.
 */
int
stereo_image_apply_lines(StereoImage *image, ZBuffer *buffer,
    unsigned int channel, unsigned int start, unsigned int end);

/**
 * A convenience macro for applying all of a z-buffer to a stereogram.
 */
#define stereo_image_apply(image, buffer, channel) \
    stereo_image_apply_lines(image, buffer, channel, 0, image->image->height)

#endif
