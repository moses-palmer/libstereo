#ifndef STEREO_EFFECT_H
#define STEREO_EFFECT_H

#include "pattern.h"

typedef struct StereoPatternEffect StereoPatternEffect;
struct StereoPatternEffect {
    /* The target pattern */
    StereoPattern *pattern;

    /**
     * Applies the effect to the specified rows.
     */
    void (*Apply)(StereoPatternEffect *effect, int start, int end, int gstart,
        int gend);

    /**
     * Updates the internal state of the effect.
     *
     * This is called every time stereo_pattern_effet_apply is called, after
     * the calls to StereoPatternEffect::Apply.
     */
    void (*Update)(StereoPatternEffect *effect);

    /**
     * Releases the resources used by the effect. This may be NULL.
     *
     * This is called by stereo_pattern_effect_free is non-NULL. After the call
     * to this function, free is called on the effect pointer.
     */
    void (*Release)(StereoPatternEffect *effect);
};

/**
 * Applies the effect to the pattern specified.
 */
void
stereo_pattern_effect_apply(StereoPatternEffect *effect);

/**
 * A convenience macro to quickly create an effect, apply it and the free it.
 */
#define stereo_pattern_effect_run(pattern, name, ...) \
    do { \
        StereoPatternEffect *_effect = stereo_pattern_effect_##name(pattern, \
            __VA_ARGS__); \
        stereo_pattern_effect_apply(_effect); \
        stereo_pattern_effect_free(_effect); \
    } while (0)

/**
 * Frees an effect.
 */
void
stereo_pattern_effect_free(StereoPatternEffect *effect);

/**
 * Overlays a luminance wave over the pattern.
 *
 * wave_count is the number of waves to use. The time complexity grows linearly
 * to this value. The wave length of a wave is the dimension of the image
 * divided by the wave index.
 *
 * strengths is the strength of every wave.
 *
 * color_components is a bit mask of the colour components to affect.
 */
StereoPatternEffect*
stereo_pattern_effect_luminance(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, int color_components);

#endif
