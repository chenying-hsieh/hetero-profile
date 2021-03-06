#include "libperf.h"            /* standard libperf include */
#include "platform.h"
#include "config.h"

static struct platform_hetero platform;
static volatile int stop = 0;

static void
term(int sig)
{
	stop = 1;
}

int main(int argc, char *argv[])
{
	unsigned long long epoch = 50000; // micro-seconds
	unsigned long long time_elapsed = 0;
	struct timeval t1, t2;

	signal(SIGALRM, term);
	signal(SIGINT, term);

	printf("Platform name: %s\n", PLATFORM_NAME);
	printf("Numer of profiling targets: %d\n", NR_PROFILES);
	if (platform_init(&platform)) {
		fprintf(stderr, "Platform init error.\n");
		return -1;
	}
	while (!stop) {
		usleep(epoch - time_elapsed);
		gettimeofday(&t1, NULL);
		PROF_LOG("time=%ld\n", t1.tv_sec * 1000000 + t2.tv_usec);
		platform_profile(&platform);
		gettimeofday(&t2, NULL);
		time_elapsed = (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
	}
	platform_profile_dump(&platform);
	return 0;
}
