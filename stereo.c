#include <stdlib.h>

#include "private.h"

#include "para.h"
#include "stereo.h"

StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern)
{
    StereoImage *result;

    if (!pattern) {
        return NULL;
    }

    result = malloc(sizeof(StereoImage));
    result->image = stereo_pattern_create(width, height);
    result->pattern = pattern;

    return result;
}

typedef struct {
    StereoImage *image;
    ZBuffer *buffer;
    unsigned int channel;

    /* The calculated offsets depth values from the buffer will yield */
    int offsets[256];
} StereoImageApplyData;

static int
stereo_image_apply_lines_do(StereoImageApplyData *data, int start, int end,
    int gstart, int gend)
{
    unsigned int x, y;
    StereoImage *image = data->image;
    ZBuffer *buffer = data->buffer;
    PatternPixel *d = stereo_pattern_row_get(image->image, start);

    for (y = start; y < end; y++) {
        unsigned char *s = stereo_zbuffer_row_get(buffer, y);

        for (x = 0; x < image->image->width; x++) {
            /* TODO: Implament */
        }
    }
}

int
stereo_image_apply_lines(StereoImage *image, ZBuffer *buffer,
    unsigned int channel, double depth, unsigned int start, unsigned int end)
{
    StereoImageApplyData data;
    int i;

    /* Verify the dimensions of the Z-buffer */
    if (image->image->width != buffer->width
            || image->image->height != buffer->height) {
        return 0;
    }

    /* Verify that the rows are valid */
    if (start >= end || end > image->image->height) {
        return 0;
    }

    /* Verify that the channel is valid */
    if (channel >= buffer->channels) {
        return 0;
    }

    data.image = image;
    data.buffer = buffer;
    data.channel = channel;

    /* Create the table of offsets */
    for (i = 0; i < sizeof(data.offsets); i++) {
        data.offsets[i] = mkfix((int)(depth * (i - sizeof(data.offsets) / 2)));
    }

    para_execute(&data, start, end,
        (ParaCallback)stereo_image_apply_lines_do);

    return 1;
}
