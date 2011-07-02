#ifndef STEREO_H
#define STEREO_H

#include "pattern.h"
#include "zbuffer.h"

typedef struct {
    /* The actual image data */
    StereoPattern *image;

    /* The pattern used */
    StereoPattern *pattern;

    /* The offsets applied to values in the z-buffer */
    int offsets[256];
} StereoImage;

#define STEREO_OFFSET_COUNT \
    (sizeof(((StereoImage*)0)->offsets) / sizeof(((StereoImage*)0)->offsets[0]))

/**
 * Creates a stereogram image with the specified dimensions.
 *
 * If you already have the z-buffer, use stereo_image_create_from_zbuffer
 * instead, since that macro uses the correct values for the width
 *
 * @param width
 *     The width of the stereo image.
 * @param height
 *     The height of the stereo image.
 * @param pattern
 *     The background pattern. This function will fail if it is not specified.
 *     Ownership of this pattern is assumed by the stereo image, and the stereo
 *     image frees it when it is freed.
 * @param strength
 *     The strength of the effect.
 * @param is_inverted
 *     Whether z-buffer values are inverted, that is, 255 means far away and 0
 *     means near.
 * @return a new stereo image, or NULL upon failure
 */
StereoImage*
stereo_image_create(unsigned int width, unsigned int height,
    StereoPattern *pattern, double strength, int is_inverted);

/**
 * Creates a stereogram image with dimensions taken from a z-buffer.
 *
 * @param zbuffer
 *     The z-buffer that determines the dimensions of the stereo image.
 * @param ...
 *     Parameters passed on to stereo_image_create.
 * @see stereo_image_create
 */
#define stereo_image_create_from_zbuffer(zbuffer, ...) \
    stereo_image_create((zbuffer)->width, (zbuffer)->height, __VA_ARGS__)

/**
 * Frees a stereo image and its image.
 *
 * @param image
 *     The stereo image to free.
 */
void
stereo_image_free(StereoImage *image);

/**
 * Sets the strength of the stereogram effect.
 *
 * This will have effect on the next call to stereo_image_apply_lines.
 *
 * @param strength
 *     The strength of the effect.
 * @param is_inverted
 *     Whether z-buffer values are inverted, that is, 255 means far away and 0
 *     means near.
 */
void
stereo_image_set_strength(StereoImage *image, double strength, int is_inverted);

/**
 * Applies a z-buffer to the stereo image, creating an actual stereogram.
 *
 * Pixels on the left will be slightly distorted until the column of the pattern
 * width is reached. This is caused by the algorithm enforcing that the leftmost
 * z-buffer column is 0, and that it then smoothly rises to its actual value.
 *
 * Since the edges of the stereogram are not clearly visible because of the way
 * they are viewed, this is not considered a serious issue.
 *
 * @param image
 *     The stereo image for which to to create a stereogram.
 * @param buffer
 *     The z-buffer to use. Its dimensions must match the dimensions of the
 *     stereo image exactly, otherwise this funciton will fail.
 * @param channel
 *     Only this channel will be used when extracting z-values from the buffer.
 * @param start
 *     The first line to touch. Lines before this one will not be modified. This
 *     must be less than end.
 * @param end
 *     The last line to touch. This line and lines after it will not be
 *     modified. This must be less than or equal to the height of the stereo
 *     image.
 * @return non-zero upon success or 0 otherwise
 */
int
stereo_image_apply_lines(StereoImage *image, ZBuffer *buffer,
    unsigned int channel, unsigned int start, unsigned int end);

/**
 * A convenience macro for applying all of a z-buffer to a stereogram.
 *
 * @see stereo_image_apply_lines
 */
#define stereo_image_apply(_image, buffer, channel) \
    stereo_image_apply_lines(_image, buffer, channel, 0, _image->image->height)

#endif
