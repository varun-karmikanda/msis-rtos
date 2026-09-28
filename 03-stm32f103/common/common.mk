# common/common.mk
#
# Shared build rules. A project's own Makefile sets PROJECT and SRCS
# (plus optional EXTRA_CFLAGS), then does:
#     include $(ROOT_DIR)/common/common.mk
#
# ROOT_DIR must point at the stm32f103/ top directory - each project's
# Makefile computes it from its own path, so it works no matter where
# `make` is invoked from.

PREFIX  ?= arm-none-eabi
CC      = $(PREFIX)-gcc
OBJCOPY = $(PREFIX)-objcopy
SIZE    = $(PREFIX)-size

MCU_FLAGS = -mcpu=cortex-m3 -mthumb

COMMON_DIR = $(ROOT_DIR)/common
STARTUP    = $(COMMON_DIR)/startup_stm32f103.s
LDSCRIPT   = $(COMMON_DIR)/stm32f103.ld
BUILD_DIR  = build

CFLAGS  = $(MCU_FLAGS) -std=gnu11 -Os -g3 -Wall -Wextra
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(COMMON_DIR)
CFLAGS += $(EXTRA_CFLAGS)

LDFLAGS  = $(MCU_FLAGS)
LDFLAGS += --specs=nano.specs --specs=nosys.specs
LDFLAGS += -nostartfiles -Wl,--gc-sections
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map

ALLSRCS := $(SRCS) $(STARTUP)
OBJS    := $(addprefix $(BUILD_DIR)/,$(notdir $(ALLSRCS)))
OBJS    := $(OBJS:.c=.o)
OBJS    := $(OBJS:.s=.o)
vpath %.c $(sort $(dir $(ALLSRCS)))
vpath %.s $(sort $(dir $(ALLSRCS)))

.PHONY: all clean flash size

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex size

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -x assembler-with-cpp -MMD -MP -c $< -o $@

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@

size: $(BUILD_DIR)/$(PROJECT).elf
	$(SIZE) $<

flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash write $< 0x8000000

clean:
	rm -rf $(BUILD_DIR)

-include $(OBJS:.o=.d)
