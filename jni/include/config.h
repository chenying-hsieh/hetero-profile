#ifndef _H_CONFIG_
#define _H_CONFIG_

#ifdef PLATFORM_QCOM_SNAPDRAGON_835
#include "platform/sd835.h"
#else
#error "Must define platform!"
#endif

#endif // _H_CONFIG_
