#include "platform.h"

int platform_init(struct platform_hetero *platform)
{
	int i;
	struct profile_ops *profile_ops = get_profile_ops();

	//init information semaphore ... sem_init(&platform->main_sem,0,0)
	sem_init(&platform->main_sem,0,0);
	platform->profile_ops = profile_ops;
	for (i = 0; i < NR_PROFILES; ++i) {
		platform->profiles[i] = profile_ops[i].create(platform);
		if (!platform->profiles[i]) {
			return -1;
		}
	}
	return 0;
}

void platform_destroy(struct platform_hetero *platform)
{
	int i;
	struct profile_ops *profile_ops = platform->profile_ops;
	for (i = 0; i < NR_PROFILES; ++i) {
		if (profile_ops[i].destroy) {
			profile_ops[i].destroy(platform->profiles[i]);
		}
	}
}

void platform_profile(struct platform_hetero *platform)
{
	int i;
	struct profile_ops *profile_ops = platform->profile_ops;

	// semaphore sig to the all device (4)
	for (i = 0; i < NR_PROFILES; ++i) {
		profile_ops[i].profile(platform->profiles[i]);
	}
	sem_wait(&platform->main_sem);
}

void platform_profile_dump(struct platform_hetero *platform)
{
	int i;
	struct profile_ops *profile_ops = platform->profile_ops;

	for (i = 0; i < NR_PROFILES; ++i) {
		profile_ops[i].dump(platform->profiles[i]);
	}
}

