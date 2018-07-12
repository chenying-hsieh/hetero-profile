#ifndef _H_PLATFORM_
#define _H_PLATFORM_

#include "config.h"

struct profile_ops {
	void *(*create)(void);
	void (*destroy)(void *profile);
	void (*profile)(void *profile);
	void (*update)(void *profile, void *profile_new); /* Update profile by profile_new */
	void (*dump)(void *profile);
};

struct platform_hetero {
	int nr_cpu;
	struct platform_cpu *cpu;
	void *profiles[NR_PROFILES];
	struct profile_ops *profile_ops;
};

int platform_init(struct platform_hetero *platform);

void platform_profile(struct platform_hetero *platform);
void platform_profile_dump(struct platform_hetero *platform);
struct profile_ops *get_profile_ops();

#endif // _H_PLATFORM_

