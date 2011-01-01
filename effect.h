#ifndef STEREO_EFFECT_H
#define STEREO_EFFECT_H

#include "pattern.h"

typedef struct StereoPatternEffect StereoPatternEffect;
struct StereoPatternEffect {
    /**
     * The target pattern
     */
    StereoPattern *pattern;

    /**
     * The name of the effect.
     */
    const char *name;

    /**
     * The iteration of the effect.
     *
     * This is incremented every time the effect is run.
     */
    unsigned int iteration;

    /**
     * Applies the effect to the specified rows.
     *
     * @param effect
     *     The effect that is being applied.
     * @param start
     *     The starting row for this run.
     * @param end
     *     The row on which to stop. This row must no be touched.
     * @param gstart
     *     The first row. This will typically be 0.
     * @param gend
     *     The last row. This will typically be the height of the target
     *     pattern.
     */
    void (*Apply)(StereoPatternEffect *effect, int start, int end, int gstart,
        int gend);

    /**
     * Updates the internal state of the effect.
     *
     * This is called every time stereo_pattern_effet_apply is called, after
     * the calls to StereoPatternEffect::Apply.
     *
     * @param effect
     *     The effect that has been applied.
     */
    void (*Update)(StereoPatternEffect *effect);

    /**
     * Releases the resources used by the effect.
     *
     * @param effect
     *     The effect to free.
     */
    void (*Release)(StereoPatternEffect *effect);
};

/**
 * Applies the effect to the pattern specified when the effect was created.
 *
 * @param effect
 *     The effect to apply.
 */
void
stereo_pattern_effect_apply(StereoPatternEffect *effect);

/**
 * A convenience macro to quickly create an effect, apply it and the free it.
 *
 * @param pattern
 *     The target pattern.
 * @param name
 *     The name of the effect. This macro uses this to generate the constructor
 *     function name for the effect by prepending stereo_pattern_effect_ to it.
 * @param ...
 *     Any arguments that the effect constructor takes.
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
 *
 * @param effect
 *     The effect to free.
 */
void
stereo_pattern_effect_free(StereoPatternEffect *effect);

/**
 * Overlays a luminance wave over the pattern.
 *
 * @param pattern
 *     The target pattern.
 * @param wave_count
 *     The number of luminance waves to use. The time complexity grows linearly
 *     with this value. The wave length of a wave is the dimension of the
 *     pattern divided by the 1 based index of the luminance wave.
 * @param strengths
 *     The strength of every luminance wave. A value of 0.0 will not create a
 *     visible wave, and a value of 1.0 will create a wave that covers all
 *     luminosity values. It must contain wave_count elements.
 * @param color_components
 *     Which colour components to affect.
 * @return a new effect
 */
StereoPatternEffect*
stereo_pattern_effect_luminance(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, int color_components);

/**
 * Renders a distorted copy of source on pattern.
 *
 * Every time the effect is applied, the waves move slighty.
 *
 * @param pattern
 *     The target pattern.
 * @param wave_count
 *     The number of waves to use. The time complexity grows linearly with this
 *     value. The wave length of a wave is the dimension of the pattern divided
 *     by the 1 based index of the wave.
 * @param strengths
 *     The strength of every wave in pairs of the horizontal and vertical
 *     strength. A value of 0.0 will not create a visible wave in the current
 *     dimension.
 * @param source
 *     The pattern that is distorted and copied to the target.
 * @return a new effect
 */
StereoPatternEffect*
stereo_pattern_effect_wave(StereoPattern *pattern, unsigned int wave_count,
    double *strengths, StereoPattern *source);

#endif
