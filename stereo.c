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

/*
 * Sets pixel to the linearly interpolated value calculated from the row.
 *
 * x is a fixed floating point value. The fractional part is used to
 * interpolate.
 *
 * width is the width of the row.
 */
static inline void
blend(PatternPixel *pixel, PatternPixel *row, int x, unsigned int width)
{
    int x1 = unmkfix(x) % width;
    int x2 = x1 + 1;
    int a1, a2;
    PatternPixel *p1, *p2;

    /* Is x1 the last row? */
    if (x2 == width) {
        x2 = 0;
    }

#ifdef MODULUS_SIGNED
    /* If modulus is signed, we need to correct for that */
    if (x1 < 0) {
        x1 += width;
    }
    if (x2 < 0) {
        x2 += width;
    }
#endif

    /* a1 is the weight of the left pixel, and a2 the weight of the right one */
    a2 = getfrac(x);
    a1 = ifrac(a2);
    p1 = &row[x1];
    p2 = &row[x2];

    /* pixel = a1 * p1 + a2 * p2 */
    pixel->r = unmkfix(mul(mkfix(p1->r), a1) + mul(mkfix(p2->r), a2));
    pixel->g = unmkfix(mul(mkfix(p1->g), a1) + mul(mkfix(p2->g), a2));
    pixel->b = unmkfix(mul(mkfix(p1->b), a1) + mul(mkfix(p2->b), a2));
    pixel->a = unmkfix(mul(mkfix(p1->a), a1) + mul(mkfix(p2->a), a2));
}

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

            blend(d, row, sourcex, width);
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
