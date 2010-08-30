#include <alloca.h>
#include <stdlib.h>
#include <string.h>

#include "private/fix.h"
#include "private/pixel.h"

#include "para/para.h"
#include "stereo.h"

StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern, double depth)
{
    StereoImage *result;
    int i;

    if (!pattern) {
        return NULL;
    }

    result = malloc(sizeof(StereoImage));
    result->image = stereo_pattern_create(width, height);
    result->pattern = pattern;

    /* Create the table of offsets */
    for (i = 0; i < 256; i++) {
        result->offsets[i] = ONE + (int)(depth * i);
    }

    return result;
}

void
stereo_image_free(StereoImage *image)
{
    stereo_pattern_free(image->image);
    stereo_pattern_free(image->pattern);

    free(image);
}

typedef struct {
    StereoImage *image;
    ZBuffer *buffer;
    unsigned int channel;
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
        int source = 0;
        unsigned char *z = stereo_zbuffer_row_get(buffer, y) + data->channel;
        PatternPixel *row = stereo_pattern_row_get(image->pattern,
            y % image->pattern->height);

        for (x = 0; x < image->image->width; x++) {
            blend2(d, row, source, image->pattern->width);
            source += data->image->offsets[*z];

            z += buffer->channels;
            d++;
        }
    }

    return 0;
}

int
stereo_image_apply_lines(StereoImage *image, ZBuffer *buffer,
    unsigned int channel, unsigned int start, unsigned int end)
{
    StereoImageApplyData data;

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

    para_execute(&data, start, end,
        (ParaCallback)stereo_image_apply_lines_do);

    return 1;
}
