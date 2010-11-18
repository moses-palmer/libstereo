#include <stdlib.h>

#include "../private/fix.h"
#include "../private/sin.h"

#include "../effect.h"

/*
 * Effect data for a luminance effect.
 */
typedef struct {
    StereoPatternEffect b;

    /* The number of waves */
    unsigned int wave_count;

    /* The strengths of the waves; it contains wave_count elements */
    int *strengths;

    /* The wave offsets; it contains wave_count elements */
    unsigned int *offsets;

    /* The pixel components */
    int components;

    /* The sin lookup table */
    SinTable hsin, vsin;
} LuminanceEffect;
#define EFFECT LuminanceEffect
#include "../private/effect.h"

/**
 * See StereoPatternEffect::Apply.
 */
static inline void
effect_apply(LuminanceEffect *effect, PatternPixel *pixel, int x, int y)
{
    int i;
    int v = 0;

    /* Add all waves together */
    for (i = 0; i < effect->wave_count; i++) {
        v += mul(effect->strengths[i], 0
            + ssin(&effect->hsin, effect->offsets[i] + x * (i + 1))
            + ssin(&effect->vsin, effect->offsets[i] + y * (i + 1)));
    }

    /* Apply the value change */
    if (effect->components & PP_RED) {
        pixel->r = cap(pixel->r + mul(pixel->r, v));
    }
    if (effect->components & PP_GREEN) {
        pixel->g = cap(pixel->g + mul(pixel->g, v));
    }
    if (effect->components & PP_BLUE) {
        pixel->b = cap(pixel->b + mul(pixel->b, v));
    }
#ifdef STEREO_ALPHA
    if (effect->components & PP_ALPHA) {
        pixel->a = cap(pixel->a + mul(pixel->a, v));
    }
#endif
}

/**
 * See StereoPatternEffect::Update.
 */
static void
effect_update(LuminanceEffect *effect)
{
    int i;

    for (i = 0; i < effect->wave_count; i++) {
        effect->offsets[i] += i;
    }
}

/**
 * See StereoPatternEffect::Release.
 */
static void
effect_release(LuminanceEffect *effect)
{
    free(effect->strengths);
    free(effect->offsets);
    sin_table_finalize(&effect->hsin);
    sin_table_finalize(&effect->vsin);
    free(effect);
}

StereoPatternEffect*
stereo_pattern_effect_luminance(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, int components)
{
    LuminanceEffect *result = malloc(sizeof(LuminanceEffect));
    int i;

    /* Initialise the basic effect data */
    stereo_effect_vt_initialize(result, pattern, luminance);

    result->wave_count = wave_count;

    /* Initialise the strengths */
    result->strengths = malloc(wave_count * sizeof(*result->strengths));
    for (i = 0; i < wave_count; i++) {
        result->strengths[i] = (int)(ONE * strengths[i]);
    }

    result->components = components;

    /* Initialise the random offsets */
    result->offsets = malloc(wave_count * sizeof(*result->offsets));
    for (i = 0; i < wave_count; i++) {
        result->offsets[i] = (unsigned int)rand();
    }

    /* Initialise the sine tables */
    sin_table_initialize(&result->hsin, pattern->width);
    sin_table_initialize(&result->vsin, pattern->height);

    return (StereoPatternEffect*)result;
}
