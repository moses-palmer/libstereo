#include <alloca.h>
#include <pthread.h>
#include <stdlib.h>

#include "para.h"

typedef struct {
    /** The mutex protecting this object */
    pthread_mutex_t mutex;

    /** The condition variable that is signalled when the event is signalled */
    pthread_cond_t cond;

    /** The state of the event */
    int signalled;
} Event;

/**
 * Initialises an event structure.
 *
 * This function must be called before an event can be used. When this function
 * has been called, event_destroy must called on the event object.
 *
 * @param event
 *     The event to initialise.
 */
static void
event_init(Event *event)
{
    pthread_mutex_init(&event->mutex, NULL);
    pthread_cond_init(&event->cond, NULL);
    event->signalled = 0;
}

/**
 * Releases the resources taken by an event.
 *
 * @param event
 *     The event to release.
 */
static void
event_destroy(Event *event)
{
    pthread_cond_destroy(&event->cond);
    pthread_mutex_destroy(&event->mutex);
}

/**
 * Waits for an event to be signalled.
 *
 * If the event is already signalled, this function returns immediately.
 *
 * @param event
 *     The event for which to wait.
 * @return 0 upon success and an error code from pthread otherwise
 */
static int
event_wait(Event *event)
{
    int result;

    /* Lock the mutex */
    result = pthread_mutex_lock(&event->mutex);
    if (result != 0) {
        return result;
    }

    /* While the event is not signalled and no error has occurred, loop */
    while (!event->signalled && result == 0) {
        result = pthread_cond_wait(&event->cond, &event->mutex);
    }
    event->signalled = 0;

    /* Do not let this call affect the return code */
    pthread_mutex_unlock(&event->mutex);

    return result;
}

/**
 * Signals an event and wakes up one waiting thread.
 *
 * @param event
 *     The event to signal.
 */
static void
event_signal(Event *event)
{
    pthread_mutex_lock(&event->mutex);
    if (!event->signalled) {
        event->signalled = 1;
        pthread_cond_signal(&event->cond);
    }
    pthread_mutex_unlock(&event->mutex);
}

typedef struct {
    /** The thread identifier */
    pthread_t thread;

    /** The event that is signalled when the parallel task should start */
    Event begin;

    /** The event that is signalled when the parallel task is completed */
    Event completed;

    /** The parallel task context that this thread belongs to */
    ParaContext *para;

    /** The start and end values for this thread; these are updated when
        para_execute is called */
    int start, end;
} ParaThread;

struct ParaContext {
    /** The mutex protecting this struct */
    pthread_mutex_t mutex;

    /** The flag that is set when the parallel task context is freed */
    int terminated;

    /** The flag that is set during a call to para_execute */
    int executing;

    /** The context data passed to the callback */
    void *context;

    /** The callback function */
    ParaCallback callback;

    /** The global start and end values; these are updated when para_execute is
        called */
    int gstart, gend;

    /** The number of entries in the threads array */
    unsigned int thread_count;

    /** The threads */
    ParaThread threads[];
};

/**
 * The cached value for the thread count.
 */
static unsigned int thread_count = 0;

/**
 * The system implementation that counts the number of CPU cores.
 *
 * This function must return a value greater than 0, otherwise it will be called
 * repeatedly.
 */
unsigned int
para_thread_count_implementation(void);

unsigned int
para_thread_count(void)
{
    char *override;

    /* If the value has been cached, return it */
    if (thread_count) {
        return thread_count;
    }

    /* Allow the environment variable to override the actual number of CPU
       cores */
    if ((override = getenv(PARA_THREAD_COUNT))
            && (thread_count = atoi(override))) {
        return thread_count;
    }

    return thread_count = para_thread_count_implementation();
}

/**
 * The thread body of a parallelised task thread.
 *
 * @param thread
 *     Data describing the current task load.
 * @return 0
 */
static int
para_execute_body(ParaThread *thread)
{
    int is_terminated;

    is_terminated = 0;
    while (!is_terminated) {
        event_wait(&thread->begin);

        /* If the parallel task context is terminating, quit this loop,
           otherwise call the callback */
        if (thread->para->terminated) {
            is_terminated = 1;
        }
        else {
            thread->para->callback(thread->para->context,
                thread->start, thread->end,
                thread->para->gstart, thread->para->gend);
        }

        event_signal(&thread->completed);
    }

    return 0;
}

ParaContext*
para_create(void *context, ParaCallback callback)
{
    unsigned int tc = para_thread_count() - 1;
    ParaContext *result = malloc(sizeof(ParaContext) + tc * sizeof(ParaThread));

    /* Make sure that the mutex is recursive */
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&result->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    /* Initialise the base data */
    result->executing = 0;
    result->terminated = 0;
    result->context = context;
    result->callback = callback;
    result->gstart = result->gend = 0;
    result->thread_count = tc;

    /* Initialise the threads */
    int i;
    for (i = 0; i < tc; i++) {
        /* Initialise the events */
        event_init(&result->threads[i].begin);
        event_init(&result->threads[i].completed);

        result->threads[i].para = result;
        result->threads[i].start = result->threads[i].end = 0;

        /* Create the suspended thread */
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&result->threads[i].thread, &attr,
            (void*)para_execute_body, &result->threads[i]);
    }

    return result;
}

void
para_free(ParaContext *para)
{
    if (!para) {
        return;
    }

    /* If we cannot lock the mutex, we fail; this is not a ParaContext? */
    if (pthread_mutex_lock(&para->mutex) != 0) {
        return;
    }

    /* Signal that we are terminted */
    para->terminated = 1;

    /* Finalise the threads; signal the event, join the thread and release the
       event */
    int i;
    for (i = 0; i < para->thread_count; i++) {
        event_signal(&para->threads[i].begin);
        pthread_join(para->threads[i].thread, NULL);
        event_destroy(&para->threads[i].completed);
        event_destroy(&para->threads[i].begin);
    }

    /* Unlock the mutex and destroy it */
    pthread_mutex_unlock(&para->mutex);
    pthread_mutex_destroy(&para->mutex);

    /* Release the memory */
    free(para);
}

void
para_execute(ParaContext *para, int start, int end)
{
    /* Get the mutex so that we may change the context variables */
    pthread_mutex_lock(&para->mutex);
    para->executing = 1;

    /* Set the start and end values */
    para->gstart = start;
    para->gend = end;

    /* Set the intervals for the threads and start them */
    int i;
    for (i = 0; i < para->thread_count; i++) {
        para->threads[i].start = (i * end) / (para->thread_count + 1);
        para->threads[i].end = ((i + 1) * end) / (para->thread_count + 1);
        event_signal(&para->threads[i].begin);
    }

    /* Run this thread */
    para->callback(para->context,
        (para->thread_count * end) / (para->thread_count + 1), end, start, end);

    /* Wait for all threads to complete this iteration */
    for (i = 0; i < para->thread_count; i++) {
        event_wait(&para->threads[i].completed);
    }

    /* Finally release the mutex */
    para->executing = 0;
    pthread_mutex_unlock(&para->mutex);
}

void
para_execute_with_context(ParaContext *para, void *context, int start, int end)
{
    /* Get the mutex so that we may change the context variables */
    pthread_mutex_lock(&para->mutex);
    para->executing = 1;

    /* With the mutex locked, change the value of the context and the revert
       it after the call to para_execute */
    void *old_context = para->context;
    para->context = context;
    para_execute(para, start, end);
    para->context = old_context;

    /* Finally release the mutex */
    para->executing = 0;
    pthread_mutex_unlock(&para->mutex);
}

int
para_is_executing(ParaContext *para)
{
    return para->executing;
}
