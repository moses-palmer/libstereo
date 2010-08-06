#include <math.h>
#include <stdlib.h>

#include "../effect.h"
#include "../private.h"

/*
 * Effect data for a wave.
 */
typedef struct {
    StereoPatternEffect b;

    /* The number of waves */
    unsigned int wave_count;

    /* The strengths of the waves; it contains wave_count elements */
    int *strengths;

    /* The wave offsets; it contains wave_count elements */
    int *offsets;

    /* The pixel components */
    int components;

    /* The sine lookup table; its element count is equal to the width of the
       pattern */
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
    d = &pattern->pixels[start * pattern->width];

    /* Iterate over all our assigned pixels */
    for (y = start; y < end; y++) {
        int z = (y * pattern->width) / pattern->height;

        for (x = 0; x < pattern->width; x++) {
            int i;
            int v = 0;

            /* Add all waves together */
            for (i = 0; i < effect->wave_count; i++) {
                v += mul(effect->strengths[i],
                    effect->sin[
                        (effect->offsets[i] + x * (i + 1)) % pattern->width]
                    + effect->sin[
                        (effect->offsets[i] + z * (i + 1)) % pattern->width]);
            }

            /* Apply the value change */
            if (effect->components & PP_RED) {
                d->r = cap(d->r + mul(d->r, v));
            }
            if (effect->components & PP_GREEN) {
                d->g = cap(d->g + mul(d->g, v));
            }
            if (effect->components & PP_BLUE) {
                d->b = cap(d->b + mul(d->b, v));
            }
            if (effect->components & PP_ALPHA) {
                d->a = cap(d->a + mul(d->a, v));
            }

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
    /* Do nothing */
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
}

StereoPatternEffect*
stereo_pattern_effect_wave(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, int components)
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
    result->strengths = malloc(wave_count * sizeof(*result->strengths));
    for (i = 0; i < wave_count; i++) {
        result->strengths[i] = (int)(LIM * strengths[i]);
    }

    result->components = components;

    /* Initialise the random offsets */
    result->offsets = malloc(wave_count * sizeof(*result->offsets));
    for (i = 0; i < wave_count; i++) {
        result->offsets[i] = rand();
    }

    /* Initialise the sine table */
    result->sin = malloc(pattern->width * sizeof(*result->sin));
    for (i = 0; i < pattern->width; i++) {
        result->sin[i] = (int)(LIM
            * sin(2 * M_PI * (double)(i + 1) / pattern->width));
    }

    return (StereoPatternEffect*)result;
}
