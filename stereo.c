#include <alloca.h>
#include <stdlib.h>
#include <string.h>

#include "private/fix.h"
#include "private/pixel.h"

#include "para/para.h"
#include "stereo.h"

StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern, double strength)
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
    for (i = 0; i < STEREO_OFFSET_COUNT; i++) {
        result->offsets[i] = (int)((strength * ONE * i)
            / (STEREO_OFFSET_COUNT - 1));
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
    int *offsets;

    offsets = alloca(image->image->width * sizeof(int));
    for (y = start; y < end; y++) {
        PatternPixel *stereo_row = (PatternPixel*)stereo_zbuffer_row_get(buffer,
            y);
        unsigned char *z = (unsigned char*)stereo_row + data->channel;
        PatternPixel *row = stereo_pattern_row_get(image->pattern,
            y % image->pattern->height);

        for (x = 0; x < image->image->width; x++) {
            int offset;

            if (x < image->pattern->width) {
                offset = 0;
            }
            else {
                offset = offsets[x - image->pattern->width];
            }
            offset += image->offsets[*z];
            offsets[x] = offset;

            blend2(d, row, mkfix(x) + offset, image->pattern->width);

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
