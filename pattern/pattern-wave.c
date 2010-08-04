#include <alloca.h>
#include <math.h>
#include <stdlib.h>

#include "../pattern.h"
#include "../para.h"
#include "../private.h"

/*
 * Parallelisation callback data for stereo_pattern_wave_overlay_do.
 */
typedef struct {
    /* The pattern to modify */
    StereoPattern *pattern;

    /* The number of waves */
    unsigned int wave_count;

    /* The strengths of the waves */
    int *strengths;

    /* The wave offsets */
    int *offsets;

    /* The pixel components */
    int components;

    /* The sine lookup table; its size is equal to the width of the pattern */
    int *sin;
} WaveData;

/*
 * The function that actually performs the wave calculation.
 */
static void
stereo_pattern_wave_overlay_do(WaveData *data, int start, int end, int gstart,
    int gend)
{
    int x, y;
    PatternPixel *d;

    /* Initialise a pointer to the first pixel */
    d = &data->pattern->pixels[start * data->pattern->width];

    /* Iterate over all our assigned pixels */
    for (y = start; y < end; y++) {
        for (x = 0; x < data->pattern->width; x++) {
            int i;
            int v = 0;

            /* Add all waves together */
            for (i = 0; i < data->wave_count; i++) {
                v += unmkfix(unmkfix(mul(data->strengths[i],
                    data->sin[
                        (data->offsets[i] + x * (i + 1)) % data->pattern->width]
                    + data->sin[
                        (data->offsets[i] + y * (i + 1)) % data->pattern->width]
                    )));
            }

            /* Apply the value change */
            if (data->components & PP_RED) {
                d->r = cap(d->r + mul(d->r, v));
            }
            if (data->components & PP_GREEN) {
                d->g = cap(d->g + mul(d->g, v));
            }
            if (data->components & PP_BLUE) {
                d->b = cap(d->b + mul(d->b, v));
            }
            if (data->components & PP_ALPHA) {
                d->a = cap(d->a + mul(d->a, v));
            }

            d++;
        }
    }
}

void
stereo_pattern_wave_overlay(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, int components)
{
    WaveData data;
    int i;

    data.pattern = pattern;
    data.wave_count = wave_count;

    /* Initialise the strengths */
    data.strengths = alloca(wave_count * sizeof(data.strengths[0]));
    for (i = 0; i < wave_count; i++) {
        data.strengths[i] = (int)(1024 * strengths[i]);
    }

    data.components = components;

    /* Initialise the random offsets */
    data.offsets = alloca(wave_count * sizeof(data.offsets[0]));
    for (i = 0; i < wave_count; i++) {
        data.offsets[i] = mkfix(rand()) / RAND_MAX;
    }

    /* Initialise the sine table */
    data.sin = alloca(pattern->width * sizeof(int));
    for (i = 0; i < pattern->width; i++) {
        data.sin[i] = (int)(1024
            * sin(2 * M_PI * (double)(i + 1) / pattern->width));
    }

    para_execute(&data, 0, pattern->height,
        (ParaCallback)stereo_pattern_wave_overlay_do);
}
