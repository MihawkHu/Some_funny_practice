#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>

#define TIME(CMD)                                                                                         \
    {                                                                                                        \
        double __t__ = time_get_current_msec();                                                          \
        CMD;                                                                                                 \
        fprintf(stderr, ">> Time used(ms): %f\n\n", time_get_current_msec() - __t__);                    \
    }

double _timeval2msec(struct timeval *tv)
{
    return tv->tv_sec * 1000.0 + tv->tv_usec * 1.0e-3;
}

double _timeval2sec(struct timeval *tv)
{
    return tv->tv_sec + tv->tv_usec * 1.0e-6;
}

double _get_timestamp(bool use_msec)
{
    struct timeval tm;

    gettimeofday(&tm, NULL);
    return ((use_msec == true) ? _timeval2msec(&tm) : _timeval2sec(&tm));
}

double time_get_current_msec()
{
    return _get_timestamp(true);
}

