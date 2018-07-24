#define _GNU_SOURCE

#include <stdlib.h>
#include<string.h>
#include<pthread.h>

#include <sched.h>
#include <unistd.h>

#include "platform.h"
#include "platform/sd835.h"
#include "libperf.h"
int profile_point_hetero[NR_PROFILES][MAX_CPU_PMU] = {
    {  LIBPERF_COUNT_HW_CPU_CYCLES, LIBPERF_COUNT_HW_INSTRUCTIONS, LIBPERF_COUNT_HW_CACHE_REFERENCES ,LIBPERF_COUNT_HW_CACHE_MISSES },
    {  LIBPERF_COUNT_HW_CPU_CYCLES, LIBPERF_COUNT_HW_INSTRUCTIONS, LIBPERF_COUNT_HW_CACHE_REFERENCES ,LIBPERF_COUNT_HW_CACHE_MISSES }
};


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
    
    return (void *)cpu_prof;
}
void *sd835_profile_cpu_control(void * profile) // for cpu control profile 
{
    struct profile_cpu * cpu_prof = (struct profile_cpu *)profile;
    while(1)
    {
        //wait profile call...
        sem_wait(cpu_prof->dev_sem);
        //call my dev threads
        for(int i=0; i<NR_CPU_CORES;i++)
        {
            sem_post(&cpu_prof->thread_sem[i]);
            printf("%d !! id !! \n",i);
        }
        //wait until threads are done...
        sem_wait(cpu_prof->dev_sem);
        
        //Am I last dev?
        if(__sync_fetch_and_add(cpu_prof->check_dev_last,1) == NR_PROFILES-1)
        {
            __sync_fetch_and_add(cpu_prof->check_dev_last,-NR_PROFILES);
            sem_post(cpu_prof->main_sem);
        }
    }
}

void *sd835_profile_cpu_thread_init(void * profile)
{
    struct profile_cpu * cpu_prof = (struct profile_cpu *)profile;
    // get thread ID (sequential number..) 0,1,2,3
    int id = (int)(__sync_fetch_and_add(&(cpu_prof->check_thread_id),1));
    // get CPU core ID
    int core_id = (cpu_prof->device_id)*NR_CPU_CORES+id+1;
    cpu_set_t mask;
    
    //init thread semaphore
    sem_init(&cpu_prof->thread_sem[id],0,0);

    // set affinity (cpu_prof->cpu_id)*NR_CPU_CORES+i
    CPU_ZERO(&mask);
    CPU_SET(core_id,&mask);
    sched_setaffinity(0, sizeof(mask),&mask);
    
    // init profile (by using libperf)
    cpu_prof->pd[id] = libperf_initialize(-1, -1);
    for(int i =0; i<MAX_CPU_PMU;i++)
        libperf_enablecounter(cpu_prof->pd[id], cpu_prof->profile_point[i]);

    while(1)
    {
        // wait for dev call...
        sem_wait(&cpu_prof->thread_sem[id]);
        // get information of profiling result...
        for(int i=0; i<MAX_CPU_PMU;i++)
            cpu_prof->pmu[id][i] = libperf_get_info(cpu_prof->pd[id],&cpu_prof->profile_point[i]);
        // print profiling result
        for(int i=0; i<MAX_CPU_PMU;i++)
            printf(" core ID is = %d  point is %d result = %llu \n",id+cpu_prof->device_id*MAX_CPU_PMU,i,cpu_prof->pmu[id][i]);
        // Am I last thread?
        if(__sync_fetch_and_add(&(cpu_prof->check_thread_last),1)==NR_CPU_CORES-1)
        {
            __sync_fetch_and_add(&(cpu_prof->check_thread_last),-NR_CPU_CORES);
            printf("finished !!!\n");
            sem_post(cpu_prof->dev_sem);
        }
    }
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
	/* TODO */
}
