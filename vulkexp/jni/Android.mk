LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_basic_r2u_1.c
LOCAL_MODULE:= vulkdev_basic_r2u_1
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_basic_r2u_2.c
LOCAL_MODULE:= vulkdev_basic_r2u_2
LOCAL_FORCE_STATIC_EXECUTABLE:= true
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_bof_r2u_1.c
LOCAL_MODULE:= vulkdev_bof_r2u_1
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_bof_r2u_2.c
LOCAL_MODULE:= vulkdev_bof_r2u_2
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_bof_r2u_3.c
LOCAL_MODULE:= vulkdev_bof_r2u_3
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_bof_rop.c
LOCAL_MODULE:= vulkdev_bof_rop
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_mmap_dkom.c
LOCAL_MODULE:= vulkdev_mmap_dkom
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_mmap_dkom.x86.c
LOCAL_MODULE:= vulkdev_mmap_dkom.x86
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_mmap_leak_1.c
LOCAL_MODULE:= vulkdev_mmap_leak_1
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_mmap_leak_2.c
LOCAL_MODULE:= vulkdev_mmap_leak_2
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_mmap_patch.c
LOCAL_MODULE:= vulkdev_mmap_patch
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_mmap_patch.x86.c
LOCAL_MODULE:= vulkdev_mmap_patch.x86
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= vulkdev_addrlimit.c
LOCAL_MODULE:= vulkdev_addrlimit
LOCAL_FORCE_STATIC_EXECUTABLE:= true
include $(BUILD_EXECUTABLE)

