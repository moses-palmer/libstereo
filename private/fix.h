#ifndef PRIVATE_FIX_H
#define PRIVATE_FIX_H

#include <limits.h>

#define INT_BITS (sizeof(int) * CHAR_BIT)

/**
 * The number of bits reserved for the decimals.
 */
#define DBITS 10

/**
 * mkfix(1).
 */
#define ONE (1 << DBITS)

/**
 * lim x, x -> 1.
 */
#define LIM (ONE - 1)

/**
 * Caps a value so that the result is 0 <= result <= 255.
 *
 * @param c
 *     An integer value.
 * @return 0 if c is less than or equal to 0, 255 if c is greater than 255 and c
 *     otherwise
 */
static inline int
cap(int c)
{
    return c < 0 ? 0 : c > 255 ? 255 : c;
}

/**
 * Creates a fixed point number.
 *
 * @param c
 *     An integer.
 * @return the fixed point number representation of c
 */
static inline int
mkfix(int c)
{
    /* Preserve the sign bit */
    return (c << DBITS) | (c & (1 << (INT_BITS - 1)));
}

/**
 * Creates an int from a fixed point number.
 *
 * @param c
 *     A fixed point number.
 * @return the integral part of c with the decimal part stripped
 */
static inline int
unmkfix(int c)
{
    /* Preserve the sign bit */
    return c >> DBITS | (c & (1 << (INT_BITS - 1)));
}

/**
 * Multiplies c with the value v.
 *
 * @param c
 *     An integer.
 * @param v
 *     A fixed point number.
 * @return a fixed point number
 */
static inline int
mul(int c, int v) {
    return unmkfix(c * v);
}

/**
 * Inverts and returns the fractional part of c.
 *
 * @param c
 *     A fixed point number.
 * @return 1 - frac(c)
 */
static inline int
ifrac(int c)
{
    return c ^ LIM;
}

/**
 * Returns the fractional part of c.
 *
 * @param c
 *     A fixed point number.
 * @return frac(c)
 */
static inline int
getfrac(int c)
{
    return c & LIM;
}

#endif
