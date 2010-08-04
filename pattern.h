#ifndef STEREO_H
#define STEREO_H

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

/**
 * Overlays a luminance wave over the pattern.
 *
 * wave_count is the number of waves to use. The time complexity grows linearly
 * to this value. The wave length of a wave is the dimension of the image
 * divided by the wave index.
 *
 * strengths is the strength of every wave.
 *
 * components is  components flags. Only pixel components in the components are
 * modified.
 */
void
stereo_pattern_wave_overlay(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, int components);

#endif
