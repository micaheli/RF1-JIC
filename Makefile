
TARGET		?= CC3D

HAL_VERSION = 1.0.4
CMSIS_VERSION = 4.5.0

F1_TARGETS = CC3D
F3_TARGETS = LUX SP3
F4_TARGETS = REVO
F7_TARGETS = F7D



#VALID_TARGETS	= $(F1_TARGETS) $(F3_TARGETS) $(F4_TARGETS) $(F7_TARGETS)

MCU_FAMILY			= STM32F1xx
MCU_MODEL_FAMILY	= stm32f103xb
MCU_MODEL			= STM32F103CB

#MCU_FAMILY		= STM32F3xx
#MCU_FAMILY		= STM32F4xx
#MCU_FAMILY		= STM32F7xx

# Working directories
ROOT		:= $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
SRC_DIR		= $(ROOT)/src/rffw
OBJECT_DIR	= $(ROOT)/obj/rffw
BIN_DIR		= $(ROOT)/obj
CMSIS		= $(ROOT)/lib/CMSIS
HAL			= $(ROOT)/lib/$(MCU_FAMILY)_HAL_Driver/$(HAL_VERSION)
INCDIR		= $(SRC_DIR)
CSRC		= 
ASMSRC		= 


################################################################################## STM32 HAL
HALSRC		+= $(notdir $(wildcard $(HAL)/Src/*.c))

EXCLUDES	= catfish.c
HALSRC		:= $(filter-out ${EXCLUDES}, $(HALSRC))

CSRC		+= $(HALSRC)
INCDIR		+= $(HAL)/Inc
################################################################################## STM32 HAL


#CSOURCES	:= $(shell find $(SRC_DIR) -name '*.c')
#VPATH		:= $(VPATH):$(CMSIS)/Include:$(CMSIS)/Device/ST/$(MCU_FAMILY)/Include


################################################################################## CMSIS
MCU_FAMILY_LOWERCASE = $(shell echo $(MCU_FAMILY) | tr '[:upper:]' '[:lower:]')
MCU_MODEL_FAMILY_LOWERCASE  = $(shell echo $(MCU_MODEL_FAMILY) | tr '[:upper:]' '[:lower:]')
MCU_MODEL_FAMILY_UPPERCASE  = $(shell echo $(MCU_MODEL_FAMILY) | tr '[:lower:]' '[:upper:]')

ASMSRC += $(CMSIS)/Device/ST/$(MCU_FAMILY)/Source/Templates/gcc/startup_$(MCU_MODEL_FAMILY_LOWERCASE).s
CSRC += $(CMSIS)/Device/ST/$(MCU_FAMILY)/Source/Templates/system_$(MCU_FAMILY_LOWERCASE).c

INCDIR += $(CMSIS)/Device/ST/$(MCU_FAMILY)/Include \
           $(CMSIS)/Include

LD_SCRIPT = $(CMSIS)/Device/ST/$(MCU_FAMILY)/Source/Templates/gcc/startup_$(MCU_MODEL_FAMILY_UPPERCASE)_FLASH.ld
################################################################################## CMSIS

CSRC	+= $(SRC_DIR)/src/main.c
CSRC	+= $(SRC_DIR)/src/stm32f1xx_it.c
CSRC	+= $(SRC_DIR)/src/system_stm32f1xx.c



































TARGET_BIN	= $(BIN_DIR)/$(TARGET).bin
TARGET_ELF	= $(OBJECT_DIR)/$(FORKNAME)_$(TARGET).elf
TARGET_OBJS	= $(addsuffix .o,$(addprefix $(OBJECT_DIR)/$(TARGET)/,$(basename $($(TARGET)_SRC))))
TARGET_DEPS	= $(addsuffix .d,$(addprefix $(OBJECT_DIR)/$(TARGET)/,$(basename $($(TARGET)_SRC))))
TARGET_MAP	= $(OBJECT_DIR)/$(FORKNAME)_$(TARGET).map

ARCH_FLAGS		= -mthumb -mcpu=cortex-m3
TARGET_FLAGS	= -D$(TARGET) -pedantic
DEVICE_FLAGS	= -DSTM32F10X_MD -DSTM32F10X

# Tool names
CC			= arm-none-eabi-gcc
OBJCOPY		= arm-none-eabi-objcopy
SIZE		= arm-none-eabi-size

OPTIMIZE	= -O2

LTO_FLAGS	= $(OPTIMIZE)

CFLAGS		 = $(ARCH_FLAGS) \
		   $(LTO_FLAGS) \
		   $(addprefix -D,$(OPTIONS)) \
		   $(addprefix -I,$(INCLUDE_DIRS)) \
		   $(DEBUG_FLAGS) \
		   -std=gnu99 \
		   -Wall -Wextra -Wunsafe-loop-optimizations -Wdouble-promotion \
		   -ffunction-sections \
		   -fdata-sections \
		   $(DEVICE_FLAGS) \
		   -DUSE_STDPERIPH_DRIVER \
		   $(TARGET_FLAGS) \
		   -D'__FORKNAME__="$(FORKNAME)"' \
		   -D'__TARGET__="$(TARGET)"' \
		   -D'__REVISION__="$(REVISION)"' \
		   -save-temps=obj \
		   -MMD -MP

ASFLAGS		 = $(ARCH_FLAGS) \
		   -x assembler-with-cpp \
		   $(addprefix -I,$(INCLUDE_DIRS)) \
		  -MMD -MP

LDFLAGS		 = -lm \
		   -nostartfiles \
		   --specs=nano.specs \
		   -lc \
		   -lnosys \
		   $(ARCH_FLAGS) \
		   $(LTO_FLAGS) \
		   $(DEBUG_FLAGS) \
		   -static \
		   -Wl,-gc-sections,-Map,$(TARGET_MAP) \
		   -Wl,-L$(LINKER_DIR) \
           -Wl,--cref \
		   -T$(LD_SCRIPT)

CPPCHECK         = cppcheck $(CSOURCES) --enable=all --platform=unix64 \
		   --std=c99 --inline-suppr --quiet --force \
		   $(addprefix -I,$(INCLUDE_DIRS)) \
		   -I/usr/include -I/usr/include/linux




TARGET_BIN	= $(BIN_DIR)/$(FORKNAME)_$(FC_VER)_$(TARGET).bin
TARGET_ELF	= $(OBJECT_DIR)/$(FORKNAME)_$(TARGET).elf
TARGET_OBJS	= $(addsuffix .o,$(addprefix $(OBJECT_DIR)/$(TARGET)/,$(basename $($(TARGET)_SRC))))
TARGET_DEPS	= $(addsuffix .d,$(addprefix $(OBJECT_DIR)/$(TARGET)/,$(basename $($(TARGET)_SRC))))
TARGET_MAP	= $(OBJECT_DIR)/$(FORKNAME)_$(TARGET).map

CLEAN_ARTIFACTS := $(TARGET_BIN)
CLEAN_ARTIFACTS += $(TARGET_HEX)
CLEAN_ARTIFACTS += $(TARGET_ELF) $(TARGET_OBJS) $(TARGET_MAP)

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex --set-start 0x8000000 $< $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET_ELF):  $(TARGET_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(SIZE) $(TARGET_ELF)



# Compile
$(OBJECT_DIR)/$(TARGET)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(CFLAGS) $<

# Assemble
$(OBJECT_DIR)/$(TARGET)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(ASFLAGS) $<

$(OBJECT_DIR)/$(TARGET)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(ASFLAGS) $<


# rebuild everything when makefile changes
$(TARGET_OBJS) : Makefile

# include auto-generated dependencies
-include $(TARGET_DEPS)





#USBD_CORE_DIR = $(ROOT)/lib/stm32/f1/STM32_USB_Device_Library/Core
#USBD_CORE_SRC = $(notdir $(wildcard $(USBD_CORE_DIR)/Src/*.c))
#USBD_CORE_INC = $(USBD_CORE_SRC)/inc

#USBD_HID_DIR = $(ROOT)/lib/stm32/f1/STM32_USB_Device_Library/Core
#USBD_HID_SRC = $(notdir $(wildcard $(USBD_HID_DIR)/Src/*.c))
#USBD_HID_INC = $(USBD_HID_SRC)/inc




