#ifndef _H_PLATFORM_
#define _H_PLATFORM_

#include <pthread.h>
#include <sched.h>
#include "config.h"

struct profile_ops {
	void *(*create)(void *platform);
	void (*destroy)(void *profile);
	void (*profile)(void *profile);
	void (*update)(void *profile, void *profile_new); /* Update profile by profile_new */
	void (*dump)(void *profile);
};

struct platform_hetero {
	int nr_cpu;
    
    sem_t main_sem; // to control main thread
    sem_t dev_sem[NR_PROFILES]; // to know devices thread jobs
    
    pthread_t thread_dv[NR_PROFILES]; // thread for each devices

    volatile int check_dev_last; // to check last dev profile..
    volatile int check_dev_id; // increase 1 and it is a ID for dev.

    void *profiles[NR_PROFILES];// information of each dev

    struct profile_ops *profile_ops; // func of each dev

    pthread_t Device_thread_id[NR_PROFILES];
};
int platform_init(struct platform_hetero *platform);

void platform_profile(struct platform_hetero *platform);
void platform_profile_dump(struct platform_hetero *platform);
struct profile_ops *get_profile_ops();

#endif // _H_PLATFORM_

