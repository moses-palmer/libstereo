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
/* effect_apply(EFFECT *effect, PatternPixel *pixel, int x, int y)            */
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
/* effect_update(SampleEffect *effect)                                        */
/* {                                                                          */
/*     ...                                                                    */
/* }                                                                          */
/*                                                                            */
/* static void                                                                */
/* effect_release(SampleEffect *effect)                                       */
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
 */
static inline void
effect_apply(EFFECT *effect, PatternPixel *pixel, int x, int y);

/**
 * Applies an effect.
 */
static void
effect_apply_lines(StereoPatternEffect *effect, int start, int end,
    int gstart, int gend)
{
    int x, y;
    StereoPattern *pattern = effect->pattern;
    PatternPixel *pixel;

    /* Initialise a pointer to the first pixel */
    pixel = stereo_pattern_row_get(pattern, start);

    /* Iterate over all our assigned pixels */
    for (y = start; y < end; y++) {
        for (x = 0; x < pattern->width; x++) {
            effect_apply((EFFECT*)effect, pixel, x, y);
            pixel++;
        }
    }
}

/**
 * Initialises an effect v-table.
 */
#define stereo_effect_vt_initialize(effect, target_pattern, namespace) \
    (effect)->b.pattern = target_pattern; \
    (effect)->b.name = #namespace; \
    (effect)->b.Apply = (void*)effect_apply_lines; \
    (effect)->b.Update = (void*)effect_update; \
    (effect)->b.Release = (void*)effect_release

#endif
