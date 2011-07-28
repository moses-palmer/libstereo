#ifndef PARA_H
#define PARA_H

/**
 * The name of the environment variable that overrides the actual number of CPU
 * cores when determining the number of threads to use.
 */
#define PARA_THREAD_COUNT "STEREO_PARA_THREAD_COUNT"

/**
 * The opaque type that represents a parallel task context.
 */
typedef struct ParaContext ParaContext;

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
 * Creates a parallel task context that may later be executed.
 *
 * para_thread_count - 1 threads are created and suspended.
 *
 * @param context
 *     The context parameter passed to all calls of the callback as the first
 *     argument.
 * @param callback
 *     The callback function that performs the task.
 * @return a parallel task context, or NULL if an error occurred
 */
ParaContext*
para_create(void *context, ParaCallback callback);

/**
 * Releases a previously created parallel task context. If para is NULL, nothing
 * is done.
 *
 * If the task is in progress, this function first waits for it to complete.
 *
 * @param para
 *     The parallel task context to free.
 */
void
para_free(ParaContext *para);

/**
 * Starts execution of a parallel task and waits for it to complete.
 *
 * All values between start and end will be passed in the intervals passed to
 * the callback function.
 *
 * If the task is already running, this function is blocking until the previous
 * task and the current task have completed.
 *
 * @param para
 *     The parallel task context to execute.
 * @param start, end
 *     The start and end values to operate on. All values, including start and
 *     end, will be passed to callbacks.
 */
void
para_execute(ParaContext *para, int start, int end);

/**
 * Starts execution of a parallel task with a temporary context and waits for it
 * to complete.
 *
 * The parallel task context data is temporarily set to context, and then
 * reverted before this function returns.
 *
 * @param para
 *     The parallel task context to execute.
 * @param context
 *     The temporary context to use.
 * @param start, end
 *     The start and end values to operate on. All values, including start and
 *     end, will be passed to callbacks.
 * @see para_execute
 */
void
para_execute_with_context(ParaContext *para, void *context, int start, int end);

/**
 * Determines whether the parallel task is currently executing.
 *
 * The return value is whether the task was executing at some point during the
 * call to this function
 *
 * @param para
 *     The parallel task context to execute.
 * @return whether the operation is in progress
 */
int
para_is_executing(ParaContext *para);

#endif
