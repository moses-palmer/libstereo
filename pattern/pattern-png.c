#include <alloca.h>
#include <png.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include "../pattern.h"
#include "../private.h"

StereoPattern*
stereo_pattern_create_from_png(FILE *in)
{
    unsigned char signature[8];
    png_structp png;
    png_infop info, end;
    png_bytep *rows = NULL;
    StereoPattern *result;
    int i, x, y;

    result = NULL;

    /* Check the signature */
    if (in) {
        if (fread(signature, 1, sizeof(signature), in) != sizeof(signature)) {
            return NULL;
        }
        if (!png_check_sig(signature, sizeof(signature))) {
            errno = EINVAL;
            return NULL;
        }
    }
    else {
        errno = EINVAL;
        return NULL;
    }

    /* Create the PNG structures */
    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(png);
    png_set_sig_bytes(png, sizeof(signature));

    /* We return here upon errors */
    if (setjmp(png_jmpbuf(png))) {
        if (result) {
            stereo_pattern_free(result);
        }
        png_destroy_read_struct(&png, &info, &end);

        return NULL;
    }

    /* Initialise the PNG struct to use in as input stream */
    png_init_io(png, in);

    /* Read the PNG and get the pixel data */
    png_read_png(png, info,
        PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_PACKING,
        NULL);
    rows = png_get_rows(png, info);
    result = stereo_pattern_create(info->width, info->height);

    /* Convert pixel data */
    for (y = 0; y < result->height; y++) {
        unsigned char *s = rows[y];
        PatternPixel *d = &result->pixels[y * result->width];

        for (x = 0; x < result->width; x++) {
            switch (info->color_type) {
            case PNG_COLOR_TYPE_GRAY:
                d->r = s[0];
                d->g = s[0];
                d->b = s[0];
                d->a = 255;
                break;

            case PNG_COLOR_TYPE_RGB:
                d->r = s[0];
                d->g = s[1];
                d->b = s[2];
                d->a = 255;
                break;

            case PNG_COLOR_TYPE_RGBA:
                d->r = s[0];
                d->g = s[1];
                d->b = s[2];
                d->a = s[3];
                break;

            case PNG_COLOR_TYPE_GA:
                d->r = s[0];
                d->g = s[0];
                d->b = s[0];
                d->a = s[1];
                break;
            }

            s += info->channels;
            d++;
        }
    }

    /* Free PNG */
    png_destroy_read_struct(&png, &info, png_infopp_NULL);

    return result;
}

int
stereo_pattern_save_to_png(StereoPattern *pattern, FILE *out)
{
    png_structp png;
    png_infop info;
    png_bytep *rows;
    int i;

    /* Create the PNG structures */
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(png);

    /* We return here upon errors */
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);

        return 0;
    }

    /* Initialise the PNG struct to use out as output stream */
    png_init_io(png, out);

    /* Initialise the information struct */
    png_set_IHDR(png, info, pattern->width, pattern->height, 8,
        PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Initialise the image data */
    rows = alloca(sizeof(png_bytep) * pattern->height);
    for (i = 0; i < pattern->height; i++) {
        rows[i] = (png_bytep)&pattern->pixels[i * pattern->width];
    }

    /* Write the image */
    png_write_info(png, info);
    png_write_image(png, rows);
    png_write_end(png, NULL);

    png_destroy_write_struct(&png, &info);

    return 1;
}
