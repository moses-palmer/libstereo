#ifndef PARA_H
#define PARA_H

/*
 * The name of the environment variable that overrides the actual number of CPU
 * cores when determining the number of threads to use.
 */
#define PARA_THREAD_COUNT "STEREO_PARA_THREAD_COUNT"

/*
 * A callback function used to parallelise a computation.
 *
 * context is the context passed to para_execute, and gstart and gend are the
 * start and end parameters passed.
 *
 * start and end are the start and end intevals calculated for the current
 * thread.
 */
typedef void (*ParaCallback)(void *context, int start, int end,
    int gstart, int gend);

/*
 * Returns the number of threads to use when parallelising a task.
 */
unsigned int
para_thread_count(void);

/*
 * Executes a pallel task using para_thread_count threads and waits for every
 * thread to complete.
 *
 * All values between start and end will be passed in the intervals passed to
 * the callback function.
 */
void
para_execute(void *context, int start, int end, ParaCallback callback);

#endif
