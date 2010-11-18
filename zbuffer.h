#ifndef STEREO_ZBUFFER_H
#define STEREO_ZBUFFER_H

#include "pattern.h"

typedef struct {
    /** The width of the z-buffer */
    unsigned int width;

    /** The height of the z-buffer */
    unsigned int height;

    /** The relative offset between one row and the next */
    int rowoffset;

    /** The number of channels for every buffer element */
    unsigned int channels;

    /** The actual buffer data */
    unsigned char *data;

    /** Whether the data buffer should be freed when the z-buffer is freed */
    int free_data;
} ZBuffer;

/**
 * Creates a z-buffer.
 *
 * The rows of the buffer are aligned on sizeof(int).
 *
 * @param width
 *     The width of the z-buffer.
 * @param height
 *     The height of the z-buffer.
 * @param channels
 *     The number of channels. A z-buffer may contain an arbitrary number of
 *     channels, but the typical values are 1 for monochome source images and
 *     3 when an RGB image is used as source.
 * @return a new z-buffer
 */
ZBuffer*
stereo_zbuffer_create(unsigned int width, unsigned int height,
    unsigned int channels);

/**
 * Creates a z-buffer from pre-allocated memory.
 *
 * @param width
 *     The width of the z-buffer.
 * @param height
 *     The height of the z-buffer.
 * @param rowoffset
 *     The byte alignment of rows. The absolute value of this must be greater or
 *     equal to width * channels; it may be less than 0 if the bitmap layout is
 *     bottom first.
 * @param channels
 *     The number of channels.
 * @param data
 *     The z-buffer data to use. This data is not freed when the z-buffer is
 *     freed.
 * @return a new z-buffer
 */
ZBuffer*
stereo_zbuffer_create_from_data(unsigned int width, unsigned int height,
    int rowoffset, unsigned int channels, unsigned char *data);

/**
 * Creates a z-buffer from a pattern.
 *
 * The dimensions of the z-buffer are taken from the dimensions of the pattern,
 * and its data is shared.
 *
 * @param pattern
 *     The pattern to use.
 * @return a new z-buffer
 */
ZBuffer*
stereo_zbuffer_create_from_pattern(StereoPattern *pattern);

/**
 * Frees a z-buffer.
 *
 * If the z-buffer was created with stereo_zbuffer_create, the data buffer is
 * also freed.
 *
 * @param zbuffer
 *     The z-buffer to free.
 */
void
stereo_zbuffer_free(ZBuffer *zbuffer);

/**
 * Returns a reference to the y'th row.
 *
 * @param buffer
 *     The z-buffer to query.
 * @param y
 *     The row to retrieve. No bounds checking is performed, so make sure that
 *     y is less than buffer->height.
 * @return a pointer to an unsigned char at the beginning of the specified row
 */
#define stereo_zbuffer_row_get(buffer, y) \
    (&(buffer)->data[y * (buffer)->rowoffset])

/**
 * Returns a reference to the pixel at (x, y).
 *
 * @param x
 *     The column to retrieve. No bounds checking is performed, so make sure
 *     that x is less than buffer->width.
 * @param y
 *     The row to retrieve. No bounds checking is performed, so make sure that
 *     y is less than buffer->height.
 * @return a pointer to an unsigned char at the specified location
 */
#define stereo_zbuffer_pixel_get(buffer, x, y) \
    (&(buffer)->data[y * (buffer)->rowoffset + x * (buffer)->channels])

#endif
