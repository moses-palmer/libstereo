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
    StereoPattern *zbuffer;

    /* The calculated offsets depth values from the buffer will yield */
    int offsets[256];
} StereoImageApplyData;

static int
stereo_image_apply_do(StereoImageApplyData *data, int start, int end,
    int gstart, int gend)
{
}

int
stereo_image_apply(StereoImage *image, StereoPattern *zbuffer, double depth)
{
    StereoImageApplyData data;
    int i;

    /* Verify the dimensions of the Z-buffer */
    if (image->image->width != zbuffer->width
            || image->image->height != zbuffer->height) {
        return 0;
    }

    data.image = image;
    data.zbuffer = zbuffer;

    /* Create the table of offsets */
    for (i = 0; i < sizeof(data.offsets); i++) {
        data.offsets[i] = mkfix((int)(depth * (i - sizeof(data.offsets) / 2)));
    }

    para_execute(&data, 0, image->image->height,
        (ParaCallback)stereo_image_apply_do);

    return 1;
}
