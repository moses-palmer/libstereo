#include <math.h>
#include <stdlib.h>

#include "../private/pixel.h"

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

    /* The sine lookup table; its element count is equal to the width of the
       source */
    int *sin;
} WaveEffect;

/**
 * See StereoPatternEffect::Apply.
 */
static void
wave_apply(WaveEffect *effect, int start, int end, int gstart,
    int gend)
{
    int x, y;
    StereoPattern *pattern = effect->b.pattern;
    PatternPixel *d;

    /* Initialise a pointer to the first pixel */
    d = stereo_pattern_row_get(pattern, start);

    /* Iterate over all our assigned pixels */
    for (y = start; y < end; y++) {
        int z = mkfix(y * pattern->width) / pattern->height;
        int w = unmkfix(z);

        for (x = 0; x < pattern->width; x++) {
            int sourcex = mkfix(x);
            int sourcey = z;
            int *strength = effect->strengths;
            int *offset = effect->offsets;
            int i;
            PatternPixel *row1;
            PatternPixel *row2;

            /* Add all waves together */
            for (i = 0; i < effect->wave_count; i++) {
                sourcex += mul(*(strength++),
                    effect->sin[(w * (i + 1) + *(offset++)) % pattern->width]);
                sourcey += mul(*(strength++),
                    effect->sin[(x * (i + 1) + *(offset++)) % pattern->width]);
            }

            getrows(effect->source->pixels, sourcey, &row1, &row2,
                effect->source->width, effect->source->height);

            blend4(d, row1, row2, sourcex, sourcey, effect->source->width);

            d++;
        }
    }
}

/**
 * See StereoPatternEffect::Update.
 */
static void
wave_update(WaveEffect *effect)
{
    int i;

    for (i = 0; i < effect->wave_count * 2; i++) {
        effect->offsets[i]++;
    }
}

/**
 * See StereoPatternEffect::Release.
 */
static void
wave_release(WaveEffect *effect)
{
    free(effect->strengths);
    free(effect->offsets);
    free(effect->sin);
    free(effect);
}

StereoPatternEffect*
stereo_pattern_effect_wave(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, StereoPattern *source)
{
    WaveEffect *result = malloc(sizeof(WaveEffect));
    int i;

    /* Initialise the basic effect data */
    result->b.pattern = pattern;
    result->b.Apply = (void*)wave_apply;
    result->b.Update = (void*)wave_update;
    result->b.Release = (void*)wave_release;

    result->wave_count = wave_count;

    /* Initialise the strengths */
    result->strengths = malloc(2 * wave_count * sizeof(*result->strengths));
    for (i = 0; i < wave_count * 2; i++) {
        result->strengths[i] = (int)(LIM * strengths[i]);
    }

    result->source = source;

    /* Initialise the random offsets */
    result->offsets = malloc(2 * wave_count * sizeof(*result->offsets));
    for (i = 0; i < wave_count * 2; i++) {
        result->offsets[i] = rand();
    }

    /* Initialise the sine table */
    result->sin = malloc(pattern->width * sizeof(*result->sin));
    for (i = 0; i < pattern->width; i++) {
        result->sin[i] = (int)(LIM
            * sin(2 * M_PI * (double)i / pattern->width));
    }

    return (StereoPatternEffect*)result;
}
