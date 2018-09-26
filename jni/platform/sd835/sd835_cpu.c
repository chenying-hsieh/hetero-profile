#define _GNU_SOURCE

#include <stdlib.h>
#include<string.h>
#include<pthread.h>

#include <sched.h>
#include <unistd.h>

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<assert.h>
#include<features.h>

#include "platform.h"
#include "platform/sd835.h"
#include "libperf.h"
int profile_point_hetero[NR_PROFILES][MAX_CPU_PMU] = {
    {  LIBPERF_COUNT_HW_CPU_CYCLES, LIBPERF_COUNT_HW_INSTRUCTIONS, LIBPERF_COUNT_HW_CACHE_REFERENCES ,LIBPERF_COUNT_HW_CACHE_MISSES },
    {  LIBPERF_COUNT_HW_CPU_CYCLES, LIBPERF_COUNT_HW_INSTRUCTIONS, LIBPERF_COUNT_HW_CACHE_REFERENCES ,LIBPERF_COUNT_HW_CACHE_MISSES }
};

static void
sd835_update_cpu_util(struct profile_cpu *cpu_prof)
{
	int i;
	char buf[256];
	char *tmp;
	int idle = 0;
	int total = 0;
	int diff_idle, diff_total;

	pread(cpu_prof->stat_fd, buf, sizeof(buf), 0);

	strtok_r(buf, " ", &tmp);
	for (i = 0; i < 10; ++i) {
		char *p;
		int cycles;
		p = strtok_r(NULL, " ", &tmp);
		cycles = strtoul(p, NULL, 10);
		total += cycles;
		if (i == 3)
			idle = cycles;
	}
	diff_total = total - cpu_prof->prev_total;
	diff_idle = idle - cpu_prof->prev_idle;
	cpu_prof->prev_total = total;
	cpu_prof->prev_idle = idle;
	cpu_prof->util = 1000 * (diff_total - diff_idle) / (diff_total + 5) / 10;
	printf("cpu-util=%d\n", cpu_prof->util);
}

void *sd835_profile_cpu_init(void *platform)
{
    struct platform_hetero *pl_main = (struct platform_hetero *)platform;
    struct profile_cpu *cpu_prof;
    cpu_prof = (struct profile_cpu *)malloc(sizeof(struct profile_cpu));

    memset(cpu_prof, 0, sizeof(struct profile_cpu));
    //get device ID
    cpu_prof->device_id = __sync_fetch_and_add(&(pl_main->check_dev_id),1);
    //get profile point
    cpu_prof->profile_point = profile_point_hetero[cpu_prof ->device_id];

    //get main thread semaphore and this device semaphore
    cpu_prof->main_sem = &pl_main->main_sem;
    cpu_prof->dev_sem = &pl_main->dev_sem[cpu_prof->device_id];

    //device sem init
    sem_init(cpu_prof->dev_sem,0,0);

    //for check last profile
    cpu_prof->check_dev_last = &pl_main->check_dev_last;

    //each core has each thread
    for(int i=0;i<NR_CPU_CORES;i++) // for each CPU_CORE , make each thread.
        pthread_create(&cpu_prof->thread_id[i],NULL,sd835_profile_cpu_thread_init,(void *)cpu_prof);

    //create thread that control cpu device.
    pthread_create(&pl_main->thread_dv[cpu_prof->device_id],NULL,sd835_profile_cpu_control,(void *)cpu_prof);

    cpu_prof->stat_fd = open("/proc/stat", O_RDONLY);

    return (void *)cpu_prof;
}

void *sd835_profile_cpu_control(void * profile) // for cpu control profile
{
    struct profile_cpu *cpu_prof = (struct profile_cpu *)profile;
    while(1) {
        //wait profile call...
        sem_wait(cpu_prof->dev_sem);

        //call my dev threads
        for(int i=0; i<NR_CPU_CORES;i++)
            sem_post(&cpu_prof->thread_sem[i]);

        //wait until threads are done...
        sem_wait(cpu_prof->dev_sem);

        //Am I last dev?
        if(__sync_fetch_and_add(cpu_prof->check_dev_last,1) == NR_PROFILES-1) {
		__sync_fetch_and_add(cpu_prof->check_dev_last,-NR_PROFILES);
		sd835_update_cpu_util(cpu_prof);
		sem_post(cpu_prof->main_sem);
        }
    }
}

void *sd835_profile_cpu_thread_init(void * profile)
{
    struct profile_cpu * cpu_prof = (struct profile_cpu *)profile;

    // get thread ID (sequential number..) 0,1,2,3
    int th_id = (int)(__sync_fetch_and_add(&(cpu_prof->check_thread_id),1)); // thread ID

    // get CPU core ID
    int core_id = (cpu_prof->device_id)*NR_CPU_CORES+th_id;// cluster ID
    cpu_prof->core_id = core_id;
    cpu_set_t mask;

    //init thread semaphore
    sem_init(&cpu_prof->thread_sem[th_id],0,0);

    // set affinity (cpu_prof->cpu_id)*NR_CPU_CORES+i
    CPU_ZERO(&mask);

    CPU_SET(core_id,&mask); //0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
    printf(" core id is %d \n",core_id);
    if(sched_setaffinity(0,sizeof(cpu_set_t),&mask)<0)
        printf(" core %d is error !!! \n",core_id);

    cpu_prof->pd[th_id] = libperf_initialize(-1, -1);
    for(int i =0; i<MAX_CPU_PMU;i++)
        libperf_enablecounter(cpu_prof->pd[th_id], cpu_prof->profile_point[i]);

    //result,freq file setting
    cpu_prof->file_perf_id[th_id] = malloc(sizeof(char)*50);
    cpu_prof->file_freq_id[th_id] = malloc(sizeof(char)*50);
    sprintf(cpu_prof->file_perf_id[th_id],"./dump/CPU_core%d_result.txt",core_id);
    sprintf(cpu_prof->file_freq_id[th_id],"/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_cur_freq",core_id);
    cpu_prof->fd_result[th_id] = open(cpu_prof->file_perf_id[th_id],O_WRONLY|O_CREAT|O_TRUNC);
    cpu_prof->fd_freq[th_id] = open(cpu_prof->file_freq_id[th_id],O_RDONLY);

    while(1) {
        // wait for dev call...
        sem_wait(&cpu_prof->thread_sem[th_id]);

        // get information of profiling result...
        for(int i=0; i<MAX_CPU_PMU;i++)
            cpu_prof->pmu_cur[th_id][i] = libperf_get_info(cpu_prof->pd[th_id],&cpu_prof->profile_point[i]);

        char log[512];
	char *buf = log;
	int ret = 0;

        // write profiling result
        for(int i=0; i<MAX_CPU_PMU;i++)
        {
            ret += sprintf(buf + ret,"%llu ",cpu_prof->pmu_cur[th_id][i]-cpu_prof->pmu_past[th_id][i]);
            //write(cpu_prof->fd_result[th_id],buf,strlen(buf)+1);
            //fsync(cpu_prof->fd_result[th_id]); // write to the disk
            cpu_prof->pmu_past[th_id][i] = cpu_prof->pmu_cur[th_id][i];
        }
	//printf("c%d %s\n", core_id, log);
        // write current freq
        //read(cpu_prof->fd_freq[th_id],buf,49);
        //write(cpu_prof->fd_result[th_id],buf,strlen(buf)+1);
        //lseek(cpu_prof->fd_freq[th_id],0,0);
        // Am I last thread?
        if(__sync_fetch_and_add(&(cpu_prof->check_thread_last),1)==NR_CPU_CORES-1) {
            __sync_fetch_and_add(&(cpu_prof->check_thread_last),-NR_CPU_CORES);
            sem_post(cpu_prof->dev_sem);
        }
    }
    close(cpu_prof->fd_result[th_id]);
}

void sd835_profile_cpu_profile(void *profile)
{
    struct profile_cpu * cpu_prof = (struct profile_cpu *)profile;
    sem_post(cpu_prof->dev_sem);
}

void sd835_profile_cpu_sync(void *profile)
{
    /* TODO:
    wait-for-signal
    */
}

void sd835_profile_cpu_update(void *profile, void *profile_new) // signal and update.
{
	/* TODO */
}

void sd835_profile_cpu_dump(void *profile)
{

}
