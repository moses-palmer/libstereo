#ifndef PARA_H
#define PARA_H

/**
 * The name of the environment variable that overrides the actual number of CPU
 * cores when determining the number of threads to use.
 */
#define PARA_THREAD_COUNT "STEREO_PARA_THREAD_COUNT"

/**
 * A callback function used to parallelise a computation.
 *
 * @param context
 *     The context passed to para_execute,.
 * @param start, end
 *     The start and end values for this parallel call.
 * @param gstart, gend
 *     The start and end values for the entire operation.
 */
typedef void (*ParaCallback)(void *context, int start, int end,
    int gstart, int gend);

/**
 * Returns the number of threads to use when parallelising a task.
 *
 * This function will first check a cached value. If it is greater than 0, it is
 * used and the function returns immediately.
 *
 * It will then check the value of the environment variable PARA_THREAD_COUNT.
 * If this is a number greater than 0, it is cached and returned.
 *
 * Otherwise it will call a platform dependent function and cache its result.
 *
 * @return the number of threads to start when parallelising a task
 */
unsigned int
para_thread_count(void);

/**
 * Executes a pallel task using para_thread_count threads and waits for every
 * thread to complete.
 *
 * All values between start and end will be passed in the intervals passed to
 * the callback function.
 *
 * @param context
 *     The context parameter passed to all calls of the callback as the first
 *     argument.
 * @param start, end
 *     The start and end values to operate on. All values, including start and
 *     end, will be passed to callbacks.
 * @param callback
 *     The callback function that performs the task.
 */
void
para_execute(void *context, int start, int end, ParaCallback callback);

#endif
