#ifndef PRIVATE_SIN_H
#define PRIVATE_SIN_H

#include <math.h>

#include "fix.h"

/**
 * A table containing precalculated sin values. It covers an entire period.
 */
typedef struct {
    int count;
    int *sin;
} SinTable;

/**
 * Initialises a sin table with count elements and precalculates its values.
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
 */
static inline void
sin_table_finalize(SinTable *sin_table)
{
    free(sin_table->sin);
}

/**
 * Calculates the sin value of 2 * pi * x / sin_table->count.
 */
static inline int
ssin(SinTable *sin_table, int x)
{
    if (x < 0) {
        return sin_table->sin[x % sin_table->count + sin_table->count];
    }
    else {
        return sin_table->sin[x % sin_table->count];
    }
}

#endif
