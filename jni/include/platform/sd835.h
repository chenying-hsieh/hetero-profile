#ifndef _H_QCOM_SNAPDRAGON_835
#define _H_QCOM_SNAPDRAGON_835

#define PLATFORM_NAME "Qualcomm Snapdragon 835"

#define NR_CPU_CORES 4
#define MAX_CPU_PMU 4
#define MAX_GPU_PE 4

enum profile_type {
	PROFILE_CPU_LITTLE = 0,
	PROFILE_CPU_BIG,
/*//  TODO
	PROFILE_GPU,
	PROFILE_DSP,
	PROFILE_MEM,
	PROFILE_THERMAL,
*/
	NR_PROFILES
};

/*
 * CPU profiling interfaces
 */
struct profile_cpu {
	int util;
	int freq;
	unsigned long long pmu[NR_CPU_CORES][MAX_CPU_PMU];
};
void sd835_profile_destroy(void *profile); /* generic destroy */

void *sd835_profile_cpu_init();
void sd835_profile_cpu_destroy(void *profile);
void sd835_profile_cpu_profile(void *profile);
void sd835_profile_cpu_update(void *profile, void *profile_new);
void sd835_profile_cpu_dump(void *profile);

/*
 * GPU profiling interfaces
 */
struct profile_gpu {
	int util;
	int freq;
};
void *sd835_profile_gpu_init();
void sd835_profile_gpu_destroy(void *profile);
void sd835_profile_gpu_profile(void *profile);
void sd835_profile_gpu_update(void *profile, void *profile_new);
void sd835_profile_gpu_dump(void *profile);

/*
 * DSP profiling interfaces
 */
struct profile_dsp {
	int util;
	int freq;
};
void *sd835_profile_dsp_init();
void sd835_profile_dsp_destroy(void *profile);
void sd835_profile_dsp_profile(void *profile);
void sd835_profile_dsp_update(void *profile, void *profile_new);
void sd835_profile_dsp_dump(void *profile);

#endif // _H_QCOM_SNAPDRAGON_835

