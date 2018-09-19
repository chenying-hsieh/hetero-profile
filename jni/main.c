#include "libperf.h"            /* standard libperf include */
#include "platform.h"

static struct platform_hetero platform;
static volatile int stop = 0;


void term(int sig)
{
	stop = 1;
}

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

	signal(SIGALRM, term);
	signal(SIGINT, term);
	while (!stop) {
		usleep(epoch - time_elapsed);

		gettimeofday(&t1, NULL);
		fprintf(stderr, "start profiling %ld...\n", t1.tv_sec * 1000000 + t2.tv_usec);

		platform_profile(&platform);
		gettimeofday(&t2, NULL);

		//        platform_profile_dump(&platform);
		time_elapsed = (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
	}
	printf("gracefull exit!\n");
	return 0;
}
