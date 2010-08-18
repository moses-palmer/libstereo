#include <stdlib.h>
#include <string.h>

#include "private.h"

#include "para.h"
#include "stereo.h"

StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern, double depth)
{
    StereoImage *result;
    int frac, i;

    if (!pattern) {
        return NULL;
    }

    result = malloc(sizeof(StereoImage));
    result->image = stereo_pattern_create(width, height);
    result->pattern = pattern;

    /* Create the table of offsets, and make sure that the first offset does not
       contain a fractional part */
    frac = (int)(depth * mkfix(
            sizeof(result->offsets) / sizeof(result->offsets[0])));
    for (i = 0; i < sizeof(result->offsets) / sizeof(result->offsets[0]); i++) {
        result->offsets[i] = (int)(depth * mkfix(
            (sizeof(result->offsets) / sizeof(result->offsets[0])) - i)) - frac;
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
    int pattern_offset = mkfix(image->pattern->width);

    for (y = start; y < end; y++) {
        unsigned char *z = stereo_zbuffer_row_get(buffer, y) + data->channel;
        PatternPixel *image_row = stereo_pattern_row_get(image->image, y);
        PatternPixel *pattern_row = stereo_pattern_row_get(image->pattern,
            y % image->pattern->height);

        for (x = 0; x < image->image->width; x++) {
            int offset = data->image->offsets[*z];
            int sourcex = mkfix(x) - pattern_offset - offset;
            PatternPixel *row;
            unsigned int width;

            if (sourcex < 0) {
                sourcex += pattern_offset + data->image->offsets[0];
                row = pattern_row;
                width = image->pattern->width;
            }
            else {
                row = image_row;
                width = image->image->width;
            }

            blend2(d, row, sourcex, width);
            z += buffer->channels;
            d++;
        }
    }
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
