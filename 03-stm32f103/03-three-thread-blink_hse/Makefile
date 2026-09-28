ROOT_DIR     := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
PROJECT      := three-thread-blink_hse

FREERTOS_DIR := $(ROOT_DIR)/FreeRTOS

SRCS := main.c
SRCS += $(FREERTOS_DIR)/tasks.c
SRCS += $(FREERTOS_DIR)/list.c
SRCS += $(FREERTOS_DIR)/queue.c
SRCS += $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c
SRCS += $(FREERTOS_DIR)/portable/MemMang/heap_4.c
# timers.c intentionally omitted - configUSE_TIMERS is 0 in FreeRTOSConfig.h

EXTRA_CFLAGS := -I$(FREERTOS_DIR)/include
EXTRA_CFLAGS += -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3
EXTRA_CFLAGS += -I.

include $(ROOT_DIR)/common/common.mk
