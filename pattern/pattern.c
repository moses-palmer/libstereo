#include <malloc.h>
#include <string.h>

#include "../pattern.h"

StereoPattern*
stereo_pattern_create(unsigned int width, unsigned int height)
{
    StereoPattern *result = malloc(sizeof(StereoPattern)
        + width * height * sizeof(PatternPixel));
    int x, y;
    PatternPixel *d;

    result->width = width;
    result->height = height;
    d = result->pixels;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            d->r = 128;
            d->g = 128;
            d->b = 128;
            d->a = 255;
            d++;
        }
    }

    return result;
}

void
stereo_pattern_free(StereoPattern *pattern)
{
    free(pattern);
}
