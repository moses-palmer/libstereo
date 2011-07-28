#include <stdlib.h>

#include "../para/para.h"
#include "../effect.h"

typedef struct {
    StereoPatternEffect b;
    ParaContext *para;
} GenericEffect;

/**
 * Retrieves the parallel task context from an effect.
 *
 * This macro relies on STEREO_PATTERN_EFFECT_HEADER being the first field of
 * the effect definition.
 */
#define stereo_pattern_effect_para(effect) \
    ((GenericEffect*)effect)->para

void
stereo_pattern_effect_apply(StereoPatternEffect *effect)
{
    para_execute(stereo_pattern_effect_para(effect),
        0, effect->pattern->height);
    effect->Update(effect);
    effect->iteration++;
}

void
stereo_pattern_effect_free(StereoPatternEffect *effect)
{
    para_free(stereo_pattern_effect_para(effect));
    effect->Release(effect);
}
