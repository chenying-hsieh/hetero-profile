#include <stdlib.h>
#include "platform/sd835.h"

void *sd835_profile_cpu_init()
{
	struct profile_cpu *cpu_prof;
	cpu_prof = (struct profile_cpu *)malloc(sizeof(struct profile_cpu));
	if (!cpu_prof) {
		return NULL;
	}
	memset(cpu_prof, 0, sizeof(struct profile_cpu));
	return cpu_prof;
}

void sd835_profile_cpu_profile(void *profile)
{
	/* TODO */
}

void sd835_profile_cpu_update(void *profile, void *profile_new)
{
	/* TODO */
}

void sd835_profile_cpu_dump(void *profile)
{
	/* TODO */
}
