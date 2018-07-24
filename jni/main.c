#define _GNU_SOURCE

#include <inttypes.h>           /* for PRIu64 definition */
#include <stdint.h>             /* for uint64_t and PRIu64 */
#include <stdio.h>              /* for printf family */
#include <stdlib.h>             /* for EXIT_SUCCESS definition */
#include "libperf.h"            /* standard libperf include */
#include <unistd.h>
#include <pthread.h>

#include <assert.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "platform.h"


static struct platform_hetero platform;

int main(int argc, char *argv[])
{
	unsigned long long epoch = 100000; // micro-seconds -> 100ms
	struct timeval t1, t2;
    printf("Platform name: %s\n", PLATFORM_NAME);
	printf("Numer of profiling targets: %d\n", NR_PROFILES);

	if (platform_init(&platform)) {
		fprintf(stderr, "Platform init error.\n");
		return -1;
	}
    //platform nr -> thread
    unsigned long long time_elapsed=0;
	
    while (1) {
		usleep(epoch - time_elapsed);

		gettimeofday(&t1, NULL);
		fprintf(stderr, "start profiling %ld...\n", t1.tv_sec * 1000000 + t2.tv_usec);
		
        platform_profile(&platform);
		gettimeofday(&t2, NULL);

                    //we don't need this//
            //        /* wait for profling is done */
            //		platform_profile_sync(&platform);

                    /* TODO: use profiling results */
            //		platform_profile_dump(&platform);

		time_elapsed = (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
	}
	return 0;
}
