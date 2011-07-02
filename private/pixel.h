#ifndef PRIVATE_PIXEL_H
#define PRIVATE_PIXEL_H

#include "../pattern.h"

#include "fix.h"

#define _MODULUS_UNSIGNED

/**
 * Returns the row at iy = unmkfix(y) and the next row.
 *
 * Rows wrap around, so if iy == height - 1, row2 will be set to the first row.
 *
 * @param pixels
 *     The start of the pixel data.
 * @param y
 *     The row to retrieve. This is a fixed floating point value.
 * @param row1, row2
 *     The output rows.
 * @param width, height
 *     The dimensions of the pixel data.
 */
static inline void
getrows(PatternPixel *pixels, int y, PatternPixel **row1,
    PatternPixel **row2, int width, int height)
{
    int i = unmkfix(y) % height;

#ifndef MODULUS_UNSIGNED
    /* If modulus is signed, we need to correct for that */
    if (i < 0) {
        i += height;
    }
#endif

    *row1 = &pixels[i * width];
    if (i == height - 1) {
        *row2 = pixels;
    }
    else {
        *row2 = *row1 + width;
    }
}

/**
 * Sets pixel to the linearly interpolated value calculated from the row at
 * ix = unmkfix(x) and the next column.
 *
 * Columns wrap around, so if if ix == width - 1, the first and the last columns
 * will be blended.
 *
 * @param pixel
 *     The pixel to set.
 * @param row
 *     The row.
 * @param x
 *     The column to retrieve. This is a fixed floating point value.
 * @param width
 *     The width of the row data.
 */
static inline void
blend2(PatternPixel *pixel, PatternPixel *row, int x, int width)
{
    int x1 = unmkfix(x) % width;
    int x2;
    int a1, a2;
    PatternPixel *p1, *p2;

#ifndef MODULUS_UNSIGNED
    /* If modulus is signed, we need to correct for that */
    if (x1 < 0) {
        x1 += width;
    }
#endif

    /* Is x1 the last row? */
    x2 = x1 + 1;
    if (x2 == width) {
        x2 = 0;
    }

    /* a1 is the weight of the left pixel, and a2 the weight of the right one */
    a2 = getfrac(x);
    a1 = ifrac(a2);

    p1 = &row[x1];
    p2 = &row[x2];

    /* pixel = a1 * p1 + a2 * p2 */
    pixel->r = unmkfix(mul(mkfix(p1->r), a1) + mul(mkfix(p2->r), a2));
    pixel->g = unmkfix(mul(mkfix(p1->g), a1) + mul(mkfix(p2->g), a2));
    pixel->b = unmkfix(mul(mkfix(p1->b), a1) + mul(mkfix(p2->b), a2));
#ifdef STEREO_ALPHA
    pixel->a = unmkfix(mul(mkfix(p1->a), a1) + mul(mkfix(p2->a), a2));
#endif
}

/**
 * Sets pixel to the linearly interpolated value calculated from the rows at
 * ix = unmkfix(x) and iy = unmkfix(y).
 *
 * Columns and rows wrap around just like in getrows and blend2.
 *
 * @param pixel
 *     The pixel to set.
 * @param row1, row2
 *     The rows to use. row2 must be the row immediately after row1.
 * @param x, y
 *     The pixel to retrieve. These are a fixed floating point values.
 * @param width
 *     The width of the row data.
 */
static inline void
blend4(PatternPixel *pixel, PatternPixel *row1, PatternPixel *row2, int x,
    int y, int width)
{
    int a1, a2;
    PatternPixel p1, p2;

    blend2(&p1, row1, x, width);
    blend2(&p2, row2, x, width);

    /* a1 is the weight of the top pixel, and a2 the weight of the bottom
       one */
    a2 = getfrac(y);
    a1 = ifrac(a2);

    /* pixel = a1 * p1 + a2 * p2 */
    pixel->r = unmkfix(mul(mkfix(p1.r), a1) + mul(mkfix(p2.r), a2));
    pixel->g = unmkfix(mul(mkfix(p1.g), a1) + mul(mkfix(p2.g), a2));
    pixel->b = unmkfix(mul(mkfix(p1.b), a1) + mul(mkfix(p2.b), a2));
#ifdef STEREO_ALPHA
    pixel->a = unmkfix(mul(mkfix(p1.a), a1) + mul(mkfix(p2.a), a2));
#endif
}

#endif
