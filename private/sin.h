#ifndef PRIVATE_SIN_H
#define PRIVATE_SIN_H

#include <math.h>

#include "fix.h"

/**
 * A table containing precalculated sin values. It covers an entire period.
 */
typedef struct {
    /** The number of entries in sin */
    int count;

    /** The actual sine values */
    int *sin;
} SinTable;

/**
 * Initialises a sin table with count elements and precalculates its values.
 *
 * It does not check whether the table has already been initialised.
 *
 * @param sin_table
 *     The sine table to initialise.
 * @param count
 *     The number of sine entries to create. This affects the resolution and
 *     memory footprint of the structure.
 */
static inline void
sin_table_initialize(SinTable *sin_table, int count)
{
    int i;

    sin_table->count = count;
    sin_table->sin = malloc(count * sizeof(sin_table->sin[0]));

    for (i = 0; i < count; i++) {
        sin_table->sin[i] = (int)(ONE * sin(2.0 * M_PI * (double)i / count));
    }
}

/**
 * Frees a sin table.
 *
 * Only the sine table is freed.
 *
 * @param sin_table
 *     The sine table to free.
 */
static inline void
sin_table_finalize(SinTable *sin_table)
{
    free(sin_table->sin);
    sin_table->sin = NULL;
    sin_table->count = 0;
}

/**
 * Calculates the sin value of 2 * pi * x / sin_table->count.
 *
 * @param sin_table
 *     The sine table.
 * @param
 */
static inline int
ssin(SinTable *sin_table, int x)
{
#ifndef MODULUS_UNSIGNED
    if (x < 0) {
        return sin_table->sin[x % sin_table->count + sin_table->count];
    }
#endif

    return sin_table->sin[x % sin_table->count];
}

#endif
