#include <stdlib.h>
#include<sched.h>
#include "platform.h"
#include "platform/sd835.h"

void sd835_profile_destroy(void *profile)
{
	if (profile) {
        int a;
        free(profile);
	}
}
static struct profile_ops profile_ops[] = { // have to implement
	{ sd835_profile_cpu_init, sd835_profile_destroy, sd835_profile_cpu_profile, sd835_profile_cpu_update, sd835_profile_cpu_dump},
	{ sd835_profile_cpu_init, sd835_profile_destroy, sd835_profile_cpu_profile, sd835_profile_cpu_update, sd835_profile_cpu_dump},
	{ sd835_profile_gpu_init, sd835_profile_destroy, sd835_profile_gpu_profile, sd835_profile_gpu_update, sd835_profile_gpu_dump},
	{ sd835_profile_dsp_init, sd835_profile_destroy, sd835_profile_dsp_profile, sd835_profile_dsp_update, sd835_profile_dsp_dump}
};
struct profile_ops *get_profile_ops() { return &profile_ops[0]; }

