LOCAL_PATH:=$(call my-dir)

define all-src-files-under
$(patsubst ./%,%,$(shell cd $(LOCAL_PATH); find -L $1 -maxdepth 1 -name "*.c" -o -name "*.cpp" -and -not -name ".*"))
endef

include $(CLEAR_VARS)

LOCAL_MODULE:= vulkdev_exp
LOCAL_SRC_FILES:= \
	$(call all-src-files-under,.)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include

LOCAL_CFLAGS := -O2 -g -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-result
LOCAL_LDLIBS := -lstdc++
LOCAL_SHARED_LIBRARIES := liblog

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -DARCH_ARM
LOCAL_CPPFLAGS += -DARCH_ARM
LOCAL_SRC_FILES += $(call all-src-files-under,arm)
endif

ifeq ($(TARGET_ARCH),arm64)
LOCAL_CFLAGS += -DARCH_ARM64
LOCAL_CPPFLAGS += -DARCH_ARM64
LOCAL_SRC_FILES += $(call all-src-files-under,arm64)
endif

ifeq ($(TARGET_ARCH),x86)
LOCAL_CFLAGS += -DARCH_X86
LOCAL_CPPFLAGS += -DARCH_X86
LOCAL_SRC_FILES += $(call all-src-files-under,x86)
endif

include $(BUILD_EXECUTABLE)
