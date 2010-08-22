#include <alloca.h>
#include <pthread.h>
#include <stdlib.h>

#include "para.h"

/*
 * The cached value for the thread count.
 */
static unsigned int thread_count = 0;

/*
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

/*
 * Callback data for a thread body.
 */
typedef struct {
    /* The callback function */
    ParaCallback callback;

    /* The user specified context passed to the callback function */
    void *context;

    /* The calculated start and end of this thread */
    int start, end;

    /* The global start and end */
    int gstart, gend;
} ParaData;

/*
 * The thread body of a parallelised task thread.
 */
static int
para_execute_body(ParaData *data)
{
    data->callback(data->context, data->start, data->end, data->gstart,
        data->gend);
}

void
para_execute(void *context, int start, int end, ParaCallback callback)
{
    unsigned int tc = para_thread_count();
    ParaData *para_data = alloca(tc * sizeof(ParaData));
    pthread_t *threads = alloca((tc - 1) * sizeof(pthread_t));
    int i;

    /* Start the threads */
    for (i = 0; i < tc; i++) {
        para_data[i].callback = callback;
        para_data[i].context = context;
        para_data[i].start = (i * end) / thread_count;
        para_data[i].end = (i == tc - 1)
            ? end
            : ((i + 1) * end) / tc;
        para_data[i].gstart = start;
        para_data[i].gend = end;

        /* Only start a new thread if i is greater than 0, since we let this
           thread be the first thread */
        if (i > 0) {
            pthread_attr_t attr;

            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
            pthread_create(&threads[i - 1], &attr, (void*)para_execute_body,
                &para_data[i]);
        }
    }

    /* Run this thread */
    para_execute_body(&para_data[0]);

    /* Wait for all threads to finish */
    for (i = 1; i < tc; i++) {
        pthread_join(threads[i - 1], NULL);
    }
}
