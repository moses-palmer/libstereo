#include <stdlib.h>

#include "../zbuffer.h"

ZBuffer*
stereo_zbuffer_create(unsigned int width, unsigned int height,
    unsigned int channels)
{
    ZBuffer *result = malloc(sizeof(ZBuffer));

    result->width = width;
    result->height = height;
    result->rowoffset = (height * channels)
        + ((height * channels) % sizeof(int)
            ? sizeof(int) - (height * channels) % sizeof(int)
            : 0);
    result->channels = channels;
    result->data = malloc(result->rowoffset * height);
    result->free_data = 1;

    return result;
}

ZBuffer*
stereo_zbuffer_create_from_data(unsigned int width, unsigned int height,
    int rowoffset, unsigned int channels, unsigned char *data)
{
        ZBuffer *result = malloc(sizeof(ZBuffer));

    result->width = width;
    result->height = height;
    result->rowoffset = rowoffset;
    result->channels = channels;
    result->data = data;
    result->free_data = 0;

    return result;
}

ZBuffer*
stereo_zbuffer_create_from_pattern(StereoPattern *pattern)
{
    ZBuffer *result = malloc(sizeof(ZBuffer));

    result->width = pattern->width;
    result->height = pattern->height;
    result->rowoffset = sizeof(PatternPixel)
        * (stereo_pattern_row_get(pattern, 1)
            - stereo_pattern_row_get(pattern, 0));
    result->channels = 4;
    result->data = (unsigned char*)pattern->pixels;
    result->free_data = 0;

    return result;
}

void
stereo_zbuffer_free(ZBuffer *buffer)
{
    if (buffer->free_data) {
        free(buffer->data);
    }

    free(buffer);
}
