CFLAGS += -D__ANDROID_ARM__ -D__ANDROID__
LDFLAGS += -Wl,--undefined,"_g_modules"

LOCAL_PATH := $(call my-dir)

# Snap Dragon CPU exclusive (Qualcomm) 
#include $(CLEAR_VARS)
#LOCAL_MODULE := QSML
#LOCAL_SRC_FILES := /opt/Qualcomm/QSML-0.14.1/lp64/arm-linux-androideabi/lib/libQSML-sequential-0.14.1.so
#LOCAL_EXPORT_C_INCLUDES := /opt/Qualcomm/QSML-0.14.1/lp64/arm-linux-androideabi/include
#include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
#LOCAL_SHARED_LIBRARIES += QSML
LOCAL_C_INCLUDES := .
LOCAL_MODULE    := test_lstm_forward
LOCAL_SRC_FILES := test_lstm_forward.c
LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon -O2 $(CFLAGS)
LOCAL_CXXFLAGS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon -O2 $(CFLAGS)
LOCAL_LDFLAGS += -fPIE $(LDFLAGS)
include $(BUILD_EXECUTABLE)
