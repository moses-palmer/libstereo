#include <math.h>
#include <stdlib.h>

#include "../private/pixel.h"
#include "../private/sin.h"

#include "../effect.h"

/*
 * Effect data for a luminance effect.
 */
typedef struct {
    StereoPatternEffect b;

    /* The number of waves */
    unsigned int wave_count;

    /* The strengths of the waves; it contains wave_count * 2 elements */
    int *strengths;

    /* The source pattern to use */
    StereoPattern *source;

    /* The offsets for the different waves; it contains wave_count * 2
       elements */
    int *offsets;

    /* The sin lookup tables */
    SinTable hsin, vsin;

    /* The current iteration */
    int iteration;
} WaveEffect;
#define EFFECT WaveEffect
#include "../private/effect.h"

static inline void
effect_apply(WaveEffect *effect, PatternPixel *pixel, int x, int y)
{
    int sourcex = mkfix(x);
    int sourcey = mkfix(y);
    int *strength = effect->strengths;
    int *offset = effect->offsets;
    int i;
    PatternPixel *row1;
    PatternPixel *row2;

    /* Add all waves together */
    for (i = 0; i < effect->wave_count; i++) {
        sourcex += mul(*(strength++),
            ssin(&effect->vsin, y * (i + 1) + *(offset++)));
        sourcey += mul(*(strength++),
            ssin(&effect->hsin, x * (i + 1) + *(offset++)));
    }

    getrows(effect->source->pixels, sourcey, &row1, &row2,
        effect->source->width, effect->source->height);

    blend4(pixel, row1, row2, sourcex, sourcey, effect->source->width);
}

/**
 * See StereoPatternEffect::Update.
 */
static void
effect_update(WaveEffect *effect)
{
    int i;

    for (i = 0; i < effect->wave_count * 2; i++) {
        if (effect->iteration % (i + 1) == 0) {
            effect->offsets[i]++;
        }
    }

    effect->iteration++;
}

/**
 * See StereoPatternEffect::Release.
 */
static void
effect_release(WaveEffect *effect)
{
    free(effect->strengths);
    free(effect->offsets);
    sin_table_finalize(&effect->hsin);
    sin_table_finalize(&effect->vsin);
    free(effect);
}

StereoPatternEffect*
stereo_pattern_effect_wave(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, StereoPattern *source)
{
    WaveEffect *result = malloc(sizeof(WaveEffect));
    int i;

    /* Initialise the basic effect data */
    stereo_effect_vt_initialize(result, pattern, wave);

    result->wave_count = wave_count;

    /* Initialise the strengths */
    result->strengths = malloc(2 * wave_count * sizeof(*result->strengths));
    for (i = 0; i < wave_count * 2; i++) {
        result->strengths[i] = (int)(ONE * strengths[i]);
    }

    result->source = source;

    /* Initialise the random offsets */
    result->offsets = malloc(2 * wave_count * sizeof(*result->offsets));
    for (i = 0; i < wave_count * 2; i++) {
        result->offsets[i] = (unsigned int)rand();
    }

    /* Initialise the sine tables */
    sin_table_initialize(&result->hsin, pattern->width);
    sin_table_initialize(&result->vsin, pattern->height);

    return (StereoPatternEffect*)result;
}
