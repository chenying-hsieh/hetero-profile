LOCAL_PATH := $(call my-dir)

# This is for snapdragon
#SDCLANG := false

TARGET_PLATFORM := PLATFORM_QCOM_SNAPDRAGON_835
TARGET_SRC := \
	platform/sd835/sd835.c \
	platform/sd835/sd835_cpu.c \
	platform/sd835/sd835_gpu.c \
	platform/sd835/sd835_dsp.c

include $(CLEAR_VARS)
LOCAL_MULTILIB := 32
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := hetero-perf
LOCAL_SRC_FILES := \
    main.c platform.c $(TARGET_SRC)
LOCAL_CFLAGS := -D$(TARGET_PLATFORM)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/
LOCAL_LDLIBS :=
include $(BUILD_EXECUTABLE)

