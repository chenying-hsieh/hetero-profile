#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "platform.h"
#include "config.h"

#define GPU_LOAD_FILE "/sys/class/kgsl/kgsl-3d0/devfreq/gpu_load"

static int
get_gpu_util(int fd)
{
        char buf[64];

        int ret = pread(fd, buf, sizeof(buf), 0);
        return strtoul(buf, NULL, 10);
}

void *_mon_(void *arg)
{
	struct profile_gpu *prof = (struct profile_gpu *)arg;

	while (!prof->stop) {
		sem_wait(prof->dev_sem);
		prof->util = get_gpu_util(prof->util_fd);
		if (prof->util) {
			prof->util_total += prof->util;
			prof->util_count++;
		}
		PROF_LOG("gpu-util=%d\n", prof->util);
		sem_post(prof->main_sem);
	}
	return NULL;
}

void *sd835_profile_gpu_init(void *arg)
{
	struct platform_hetero *platform = (struct platform_hetero *)arg;
	struct profile_gpu *prof;

	prof = malloc(sizeof(struct profile_gpu));
	if (!prof)
		return prof;

	memset(prof, 0, sizeof(struct profile_gpu));
	prof->stop = 0;
        prof->util_fd = open(GPU_LOAD_FILE, O_RDONLY);
	prof->main_sem = &platform->main_sem;
	prof->dev_sem = &platform->dev_sem[PROFILE_GPU];
	prof->util_count = 0;
	prof->util_total = 0;
	sem_init(prof->dev_sem, 0, 0);

	if (pthread_create(&prof->thread, NULL, _mon_, (void *)prof) ||
	    pthread_detach(prof->thread)) {
	    printf("GPU thread creation failed.\n");
	    exit(-1);
	}
	return prof;
}

void sd835_profile_gpu_profile(void *profile)
{
	struct profile_gpu *prof = (struct profile_gpu *)profile;
	sem_post(prof->dev_sem);
}

void sd835_profile_gpu_update(void *profile, void *profile_new)
{
	/* TODO */
}

void sd835_profile_gpu_dump(void *profile)
{
	struct profile_gpu *prof = (struct profile_gpu *)profile;
	if (!prof->util_count)
		prof->util_count = 1;
	printf("avg-gpu-util=%.2f\n", prof->util_total / (float)prof->util_count);
}

void sd835_profile_gpu_run(void *profile)
{
	/* TODO */
}
