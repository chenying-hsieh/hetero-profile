#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "platform.h"

static struct platform_hetero platform;

int main(int argc, char *argv[])
{
	unsigned long long epoch = 100000; // micro-seconds
	struct timeval t1, t2;

	printf("Platform name: %s\n", PLATFORM_NAME);
	printf("Numer of profiling targets: %d\n", NR_PROFILES);

	if (platform_init(&platform)) {
		fprintf(stderr, "Platform init error.\n");
		return -1;
	}
	while (1) {
		unsigned long long time_elapsed;

		gettimeofday(&t1, NULL);
		fprintf(stderr, "start profiling %ld...\n", t1.tv_sec * 1000000 + t2.tv_usec);
		platform_profile(&platform);
		platform_profile_dump(&platform);
		gettimeofday(&t2, NULL);

		time_elapsed = (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
		usleep(epoch - time_elapsed);
	}
	return 0;
}
