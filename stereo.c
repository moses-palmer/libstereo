#include <alloca.h>
#include <stdlib.h>
#include <string.h>

#include "private/fix.h"
#include "private/pixel.h"

#include "para/para.h"
#include "stereo.h"

StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern, double strength, int is_inverted)
{
    StereoImage *result;

    if (!pattern) {
        return NULL;
    }

    result = malloc(sizeof(StereoImage));
    result->image = stereo_pattern_create(width, height);
    result->pattern = pattern;

    stereo_image_set_strength(result, strength, is_inverted);

    return result;
}

void
stereo_image_free(StereoImage *image)
{
    stereo_pattern_free(image->image);
    stereo_pattern_free(image->pattern);

    free(image);
}

void
stereo_image_set_strength(StereoImage *image, double strength, int is_inverted)
{
    int i;

    /* Create the table of offsets */
    for (i = 0; i < STEREO_OFFSET_COUNT; i++) {
        int ival = is_inverted ? STEREO_OFFSET_COUNT - i : i;
        image->offsets[i] = (int)((strength * ONE * ival)
            / (STEREO_OFFSET_COUNT - 1));
    }
}

typedef struct {
    StereoImage *image;
    ZBuffer *buffer;
    unsigned int channel;
} StereoImageApplyLinesData;

static int
stereo_image_apply_lines_do(StereoImageApplyLinesData *data, int start, int end,
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

            /* If we have passed the first pattern columns, the current offset
               depends on the previous offsets, otherwise we make a slope
               upwards to the value of the first column of the z-buffer */
            if (x >= image->pattern->width) {
                offset = offsets[x - image->pattern->width]
                    + image->offsets[*z];
            }
            else {
                offset = image->offsets[*z] * x / image->pattern->width;
            }
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
    StereoImageApplyLinesData data;

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
