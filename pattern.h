#ifndef STEREO_PATTERN_H
#define STEREO_PATTERN_H

#include <stdio.h>

/**
 * A pixel of the pattern.
 */
typedef struct {
    unsigned char r, g, b, a;
} PatternPixel;

/*
 * Flags for the different components of a pixel.
 */
enum {
    PP_RED   = 1 << 0,
    PP_GREEN = 1 << 1,
    PP_BLUE  = 1 << 2,
    PP_ALPHA = 1 << 3
};

/*
 * Flags for the colour components of a pixel.
 */
#define PP_COLORS (PP_RED | PP_GREEN | PP_BLUE)

typedef struct {
    /* The with of the pattern */
    unsigned int width;

    /* The height of the pattern */
    unsigned int height;

    /* The pixel data */
    PatternPixel pixels[0];
} StereoPattern;

/*
 * Returns a reference to the y'th pattern pixel row.
 */
#define stereo_pattern_row_get(pattern, y) \
    (&(pattern)->pixels[y * (pattern)->width])

/*
 * Returns a reference to the pattern pixel at (x, y).
 */
#define stereo_pattern_pixel_get(pattern, x, y) \
    (&(pattern)->pixels[y * (pattern)->width + x])

/**
 * Creates a pattern with the specified dimensions.
 *
 * Initially the pattern is gray.
 */
StereoPattern*
stereo_pattern_create(unsigned int width, unsigned int height);

/**
 * Frees a previously allocated pattern.
 */
void
stereo_pattern_free(StereoPattern *pattern);

/**
 * Creates a pattern from a PNG file. Its dimensions are determined by the
 * dimensions of the image.
 *
 * If the file cannot be read, or is not a PNG file, NULL is returned.
 *
 * Please make sure that the file is opened in binary mode.
 */
StereoPattern*
stereo_pattern_create_from_png(FILE *in);

/*
 * Saves the pattern to a file in PNG format. The return value is 0 if an error
 * occurs, and non-zero otherwise.
 *
 * Please make sure that the file is opened in binary mode.
 */
int
stereo_pattern_save_to_png(StereoPattern *pattern, FILE *out);

#endif
