/******************************************************************************/
/* How to use this file                                                       */
/* ====================                                                       */
/*                                                                            */
/* Define the internal struct used by the effect and include this file:       */
/* typedef struct {                                                           */
/*     StereoPatternEffect b;                                                 */
/*     ...                                                                    */
/* } SampleEffect                                                             */
/* #define EFFECT SampleEffect                                                */
/* #include "../private/effect.h"                                             */
/*                                                                            */
/* Implement the pixel manipulator:                                           */
/* static void                                                                */
/* effect_apply(EFFECT *effect, PatternPixel *pixel,                          */
/*     int x, int y, int dx, int dy, int rx, int ry)                          */
/* {                                                                          */
/*     pixel->r = 255;                                                        */
/*     pixel->g = 255;                                                        */
/*     pixel->b = 255;                                                        */
/* #ifdef STEREO_ALPHA                                                        */
/*     pixel->a = 255;                                                        */
/* #endif                                                                     */
/* }                                                                          */
/*                                                                            */
/* Implement the effect update and release functions:                         */
/* static void                                                                */
/* sample_update(SampleEffect *effect)                                        */
/* {                                                                          */
/*     ...                                                                    */
/* }                                                                          */
/*                                                                            */
/* static void                                                                */
/* sample_release(SampleEffect *effect)                                       */
/* {                                                                          */
/*     ...                                                                    */
/* }                                                                          */
/*                                                                            */
/* Implement the effect constructor:                                          */
/* StereoPatternEffect*                                                       */
/* stereo_pattern_effect_sample(StereoPattern *pattern, ...)                  */
/* {                                                                          */
/*     SampleEffect *result = malloc(sizeof(SampleEffect);                    */
/*                                                                            */
/*     stereo_effect_vt_initialize(result, pattern, sample);                  */
/*     ...                                                                    */
/* }                                                                          */
/******************************************************************************/

#ifndef PRIVATE_EFFECT_H
#define PRIVATE_EFFECT_H

/**
 * Applies the effect to a single pixel.
 *
 * x and y are the currect coordinates.
 *
 * dx and dy are the current coordinates as fixed point decimal numbers.
 *
 * rx and ry are the current relative coordinates. As x goes from 0 to
 * mkfix(width), rx will go from 0 to mkfix(max(width, height)), and as y goes
 * from 0 to mkfix(height), ry will go from 0 to mkfix(max(width, height)). The
 * purpose of these parameters are mainly to allow one lookup table for values
 * related to the coordinates.
 */
static void
effect_apply(EFFECT *effect, PatternPixel *pixel,
    int x, int y, int dx, int dy, int rx, int ry);

/**
 * Applies an effect when the width is greater than the height.
 */
static void
effect_hgtv_apply(StereoPatternEffect *effect, int start, int end,
    int gstart, int gend)
{
    int x, y;
    StereoPattern *pattern = effect->pattern;
    PatternPixel *pixel;

    /* Initialise a pointer to the first pixel */
    pixel = stereo_pattern_row_get(pattern, start);

    /* Iterate over all our assigned pixels */
    for (y = start; y < end; y++) {
        int dy = mkfix(y);
        int ry = (dy * pattern->width) / pattern->height;

        for (x = 0; x < pattern->width; x++) {
            int dx;
            int rx = dx = mkfix(x);

            effect_apply((EFFECT*)effect, pixel, x, y, dx, dy, rx, ry);
            pixel++;
        }
    }
}

/**
 * Applies an effect when the height is greater than the width.
 */
static void
effect_vgth_apply(StereoPatternEffect *effect, int start, int end,
    int gstart, int gend)
{
    int x, y;
    StereoPattern *pattern = effect->pattern;

    /* Iterate over all our assigned pixels */
    for (x = start; x < end; x++) {
        PatternPixel *pixel = stereo_pattern_row_get(pattern, 0) + x;
        int dx = mkfix(x);
        int rx = (dx * pattern->height) / pattern->width;

        for (y = 0; y < pattern->height; y++) {
            int dy;
            int ry = dy = mkfix(y);

            effect_apply((void*)effect, pixel, x, y, dx, dy, rx, ry);
            pixel += pattern->width;
        }
    }
}

/**
 * Initialises an effect v-table.
 */
#define stereo_effect_vt_initialize(effect, target_pattern, namespace) \
    (effect)->b.pattern = target_pattern; \
    (effect)->b.name = #namespace; \
    (effect)->b.start = 0; \
    (effect)->b.end = ((target_pattern)->height > (target_pattern)->width) \
        ? (target_pattern)->width : (target_pattern)->height; \
    (effect)->b.Apply = ((target_pattern)->height > (target_pattern)->width) \
        ? (void*)effect_vgth_apply : (void*)effect_hgtv_apply; \
    (effect)->b.Update = (void*)namespace##_update; \
    (effect)->b.Release = (void*)namespace##_release

#endif
