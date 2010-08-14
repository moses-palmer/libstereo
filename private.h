#ifndef STEREO_PRIVATE_H
#define STEREO_PRIVATE_H

#if defined __x86_64__
# define INT_BITS 64
#else
# define INT_BITS 32
#endif

/*
 * The number of bits reserved for the decimals.
 */
#define DBITS 16

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

#endif
