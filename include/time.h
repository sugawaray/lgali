#include <types.h>

struct timespec {
	time_t	tv_sec;
	long	tv_nsec;
};

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
