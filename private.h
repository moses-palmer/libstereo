#ifndef STEREO_PRIVATE_H
#define STEREO_PRIVATE_H

#include "pattern.h"

#if defined __x86_64__
# define INT_BITS 64
#else
# define INT_BITS 32
#endif

/*
 * The number of bits reserved for the decimals.
 */
#define DBITS 12

/*
 * lim x, x -> 1.
 */
#define LIM ((1 << DBITS) - 1)

/*
 * Caps a value so that the result is 0 <= result <= 255.
 */
static inline int
cap(int c)
{
    return c < 0 ? 0 : c > 255 ? 255 : c;
}

/*
 * Creates a fixed point number.
 */
static inline int
mkfix(int c)
{
    /* Preserve the sign bit */
    if (c < 0) {
        return -(-c << DBITS);
    }
    else {
        return c << DBITS;
    }
}

/*
 * Creates a byte from a fixed point number.
 */
static inline int
unmkfix(int c)
{
    /* Preserve the sign bit */
    if (c < 0) {
        return -(-c >> DBITS);
    }
    else {
        return c >> DBITS;
    }
}

/*
 * Multiplies c with the value v, which is assumed to be a fixed point value
 * with DBITS bits reserved for the decimals.
 */
static inline int
mul(int c, int v) {
    return unmkfix(c * v);
}

/*
 * Returns the fractional part of c.
 */
static inline int
getfrac(int c)
{
    return c & LIM;
}

/*
 * Inverts the fractional part of c.
 */
static inline int
ifrac(int c)
{
    return c ^ LIM;
}

/*
 * Sets pixel to the linearly interpolated value calculated from the row.
 *
 * x is a fixed floating point value. The fractional part is used to
 * interpolate.
 *
 * width is the width of the row.
 */
static inline void
blend2(PatternPixel *pixel, PatternPixel *row, int x, unsigned int width)
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

/*
 * Sets pixel to the linearly interpolated value calculated from the rows.
 *
 * x and y are fixed floating point values. The fractional part is used to
 * interpolate.
 *
 * width is the width of the rows.
 */
static inline void
blend4(PatternPixel *pixel, PatternPixel *row1, PatternPixel *row2, int x,
    int y, unsigned int width)
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
    pixel->a = unmkfix(mul(mkfix(p1.a), a1) + mul(mkfix(p2.a), a2));
}

#endif
