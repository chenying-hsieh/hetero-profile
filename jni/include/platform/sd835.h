#include <sched.h>

#include <unistd.h>

#include <assert.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>



#include <semaphore.h>
#include <pthread.h>

//sean
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <getopt.h>
#include <pthread.h>

#include "libperf.h"

#ifndef _H_QCOM_SNAPDRAGON_835
#define _H_QCOM_SNAPDRAGON_835

#define PLATFORM_NAME "Qualcomm Snapdragon 835"

#define NR_CPU_CORES 4
#define MAX_CPU_PMU 4
#define MAX_GPU_PE 4

enum profile_type {
	PROFILE_CPU_LITTLE = 0,
	PROFILE_CPU_BIG,
	PROFILE_GPU,
/*//  TODO
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

    int device_id; // cpu id 0 = first cpu , id 1 = second cpu
    int core_id;
    int *profile_point; //  int profile_point_hetero[NR_PROFILES][MAX_CPU_PMU]
    //sean it is declared
    struct libperf_data *pd[MAX_CPU_PMU];// structre of libperf

    //semaphore
    sem_t* main_sem; // our device semaphore ==> &profile->main_sem;
    sem_t* dev_sem; // our device semaphore ==> &profile->dev_sem[device_id];
    sem_t thread_sem[NR_CPU_CORES]; // each core has thread (to call profile..)

    //check_Val & ID
    volatile int* check_dev_last;// to know Am I last profile device? => then sig to main proc
    volatile int check_thread_id; // first must be 0
    volatile int check_thread_last; // zero & if NR_CPU_CORES => exit!!

    //file
    int fd_result[NR_CPU_CORES];
    int fd_freq[NR_CPU_CORES];
    char *file_perf_id[4];
    char *file_freq_id[4];

    //set affinity
    void * cpu_set_aff[NR_CPU_CORES];

    //thread
    pthread_t thread_id[NR_CPU_CORES];

    int freq;
    unsigned long long pmu_cur[NR_CPU_CORES][MAX_CPU_PMU];//result
    unsigned long long pmu_past[NR_CPU_CORES][MAX_CPU_PMU];//result

    int prev_total;
    int prev_idle;
    int stat_fd;
    int util;
};

void sd835_profile_destroy(void *profile); /* generic destroy */

void *sd835_profile_cpu_control(void* profile);
void *sd835_profile_cpu_thread_init(void * profile);

void *sd835_profile_cpu_init(void *platform);
void sd835_profile_cpu_destroy(void *profile);
void sd835_profile_cpu_profile(void *profile);
void sd835_profile_cpu_update(void *profile, void *profile_new);
void sd835_profile_cpu_dump(void *profile);

/*
 * GPU profiling interfaces
 */
struct profile_gpu {
	int util_fd;
	sem_t *dev_sem;
	sem_t *main_sem;
	unsigned char stop;
	unsigned char util;

	pthread_t thread;
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

