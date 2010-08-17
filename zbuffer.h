#ifndef STEREO_ZBUFFER_H
#define STEREO_ZBUFFER_H

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

    /* Whether the data buffer should be freed when the z-buffer is freed */
    int free_data;
} ZBuffer;

/*
 * Creates a z-buffer.
 *
 * The rows of the buffer are aligned on sizeof(int).
 */
ZBuffer*
stereo_zbuffer_create(unsigned int width, unsigned int height,
    unsigned int channels);

/*
 * Frees a z-buffer.
 */
void
stereo_zbuffer_free(ZBuffer *zbuffer);

/*
 * Creates a z-buffer from pre-allocated memory.
 */
ZBuffer*
stereo_zbuffer_create_from_data(unsigned int width, unsigned int height,
    int rowoffset, unsigned int channels, unsigned char *data);

/*
 * Creates a z-buffer from a pattern.
 */
ZBuffer*
stereo_zbuffer_create_from_pattern(StereoPattern *pattern);

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

#endif
