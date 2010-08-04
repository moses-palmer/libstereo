#include <errno.h>
#include <unistd.h>

unsigned int
para_thread_count_implementation(void)
{
    long result;

    errno = 0;
    result = sysconf(_SC_NPROCESSORS_CONF);
    if (errno || result == -1) {
        return 1;
    }

    return (unsigned int)result;
}
