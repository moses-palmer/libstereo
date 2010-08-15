#ifndef STEREO_H
#define STEREO_H

#include "pattern.h"

typedef struct {
    /* The width of the z-buffer */
    unsigned int width;

    /* The height of the z-buffer */
    unsigned int height;

    /* The relative offset between one row and the next */
    int rowoffset;

    /* The number of channels for every buffer element */
    unsigned int channels;

    /* The actual buffer data */
    unsigned char *data;
} ZBuffer;

/*
 * Returns a reference to the y'th row.
 */
#define stereo_zbuffer_row_get(buffer, y) \
    (&(buffer)->data[y * (buffer)->rowoffset])

/*
 * Returns a reference to the pixel at (x, y).
 */
#define stereo_zbuffer_pixel_get(buffer, x, y) \
    (&(buffer)->data[y * (buffer)->rowoffset + x * (buffer)->channels])

typedef struct {
    /* The actual image data */
    StereoPattern *image;

    /* The pattern used */
    StereoPattern *pattern;
} StereoImage;

/**
 * Creates a stereogram image with the specified dimensions.
 *
 * pattern is used as the background pattern. This function will fail if it is
 * not specified.
 */
StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern);

/**
 * Applies a z-buffer to the stereo image, creating an actual stereogram.
 *
 * Only the channel channel is used, and only the lines from start to end are
 * modified.
 *
 * depth is the multiplicator for depths.
 *
 * The result is non-zero upon success, or 0 if the dimensions of the buffer do
 * not match the dimensions of the stereo image.
 */
int
stereo_image_apply_lines(StereoImage *image, ZBuffer *buffer,
    unsigned int channel, double depth, unsigned int start, unsigned int end);

/**
 * A convenience macro for applying all of a z-buffer to a stereogram.
 */
#define stereo_image_apply(image, buffer, channel, depth) \
    stereo_image_apply_lines(image, buffer, channel, depth, 0, \
        image->image->height)

#endif
