#ifndef STEREO_PATTERN_H
#define STEREO_PATTERN_H

#include <stdio.h>

/**
 * A pixel of the pattern.
 */
typedef struct {
    unsigned char r, g, b, a;
} PatternPixel;

/**
 * Flags for the different components of a pixel.
 */
enum {
    PP_RED   = 1 << 0,
    PP_GREEN = 1 << 1,
    PP_BLUE  = 1 << 2,
    PP_ALPHA = 1 << 3
};

/**
 * Flags for the colour components of a pixel.
 */
#define PP_COLORS (PP_RED | PP_GREEN | PP_BLUE)

typedef struct {
    /** The with of the pattern */
    unsigned int width;

    /** The height of the pattern */
    unsigned int height;

    /** The pixel data */
    PatternPixel pixels[0];
} StereoPattern;

/**
 * Creates a pattern with the specified dimensions.
 *
 * Initially the pattern is gray.
 *
 * @param width
 *     The width of the pattern.
 * @param height
 *     The height of the pattern.
 * @return a new pattern
 */
StereoPattern*
stereo_pattern_create(unsigned int width, unsigned int height);

/**
 * Creates a pattern from a PNG file.
 *
 * The dimensions of the pattern are the same as the dimensions of the image.
 *
 * @param in
 *     The file to read. Please make sure that it is opened in binary mode. If
 *     it is not a valid PNG file, the function fails.
 * @return a new pattern, or NULL upon failure
 */
StereoPattern*
stereo_pattern_create_from_png(FILE *in);

/**
 * Creates a pattern from a PNG file.
 *
 * @param filename
 *     The name of the file. If it does not exist or cannot be opened, the
 *     function fails.
 * @return a new pattern, or NULL upon failure
 * @see stereo_pattern_create_from_png
 */
StereoPattern*
stereo_pattern_create_from_png_file(const char *filename);

/**
 * Frees a previously allocated pattern.
 *
 * @param pattern
 *     The pattern to free.
 */
void
stereo_pattern_free(StereoPattern *pattern);

/**
 * Saves the pattern to a file in PNG format.
 *
 * @param pattern
 *     The pattern to save.
 * @param out
 *     The file to write the PNG to. Please make sure that it is opened in
 *     binary mode.
 * @return non-zero upon success and 0 otherwise
 */
int
stereo_pattern_save_to_png(StereoPattern *pattern, FILE *out);

/**
 * Saves the pattern to a file in PNG format.
 *
 * @param pattern
 *     The pattern to save.
 * @param filename
 *     The file to write. If this file cannot be created, the function fails.
 * @return non-zero upon success and 0 otherwise
 * @see stereo_pattern_save_to_png
 */
int
stereo_pattern_save_to_png_file(StereoPattern *pattern, const char *filename);

/**
 * Returns a reference to the y'th row.
 *
 * @param pattern
 *     The pattern to query.
 * @param y
 *     The row to retrieve. No bounds checking is performed, so make sure that
 *     y is less than pattern->height.
 * @return a pointer to a PatternPixel at the beginning of the specified row
 */
#define stereo_pattern_row_get(pattern, y) \
    (&(pattern)->pixels[y * (pattern)->width])

/**
 * Returns a reference to the pixel at (x, y).
 *
 * @param x
 *     The column to retrieve. No bounds checking is performed, so make sure
 *     that x is less than pattern->width.
 * @param y
 *     The row to retrieve. No bounds checking is performed, so make sure that
 *     y is less than pattern->height.
 * @return a pointer to a PatternPixel at the specified location
 */
#define stereo_pattern_pixel_get(pattern, x, y) \
    (&(pattern)->pixels[y * (pattern)->width + x])

#endif
