#ifndef _H_CONFIG_
#define _H_CONFIG_

#include <pthread.h>
#include <sched.h>
#ifdef PLATFORM_QCOM_SNAPDRAGON_835
#include "platform/sd835.h"
#else
#error "Must define platform!"
#endif

//#define VERBOSE_PROFILE
#ifdef VERBOSE_PROFILE
#define PROF_LOG(fmt, args...) printf(fmt, ##args)
#else
#define PROF_LOG(fmt, args...) (void)0
#endif

#endif // _H_CONFIG_
