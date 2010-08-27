#include <stdlib.h>

#include "../effect.h"
#include "../para/para.h"

void
stereo_pattern_effect_apply(StereoPatternEffect *effect)
{
    para_execute(effect, effect->start, effect->end,
        (ParaCallback)effect->Apply);
    effect->Update(effect);
}

void
stereo_pattern_effect_free(StereoPatternEffect *effect)
{
    effect->Release(effect);
}
