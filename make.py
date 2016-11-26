#!/usr/bin/env python
# -*- coding: utf-8 -*-
#Cubic spline fitter

from __future__ import print_function

import re
import sys
import threading
import subprocess
import glob
import sys
import ntpath
import os
import platform
import argparse
import ctypes as c

from collections import namedtuple

try:
    # Python 3 name
    import queue
except ImportError:
    # Python 2 name
    import Queue as queue

class ColorFallback(object):
    def __getattr__(self, attr):
        return ""

try:
    from colorama import Fore, Style
except ImportError:
    Fore = Style = ColorFallback()


# Magic code
# add new method for python string object
# used
# STM32F1_MCU_DIR = "src/rffw/target/stm32f1"
# STM32F1_MCU_DIR.path
# return for unix > src/rffw/target/stm32f1
# return for windows > src\rffw\target\stm32f1
class PyObject_HEAD(c.Structure):
    _fields_ = [
        ('HEAD', c.c_ubyte * (object.__basicsize__ - c.sizeof(c.c_void_p))),
        ('ob_type', c.c_void_p)
    ]

_get_dict = c.pythonapi._PyObject_GetDictPtr
_get_dict.restype = c.POINTER(c.py_object)
_get_dict.argtypes = [c.py_object]

def get_dict(object):
    return _get_dict(object).contents.value

@property
def get_path_method(self):
    if platform.system() == 'Windows':
        return self.replace("/", "\\")
    return self

get_dict(str)['path'] = get_path_method



this_dir = os.path.dirname(os.path.abspath(__file__))
OUTPUT_PATH = os.path.join(this_dir, "output")

parser = argparse.ArgumentParser(description='')
parser.add_argument('-C', "--clean", help="clean up output folder", action='store_true')
parser.add_argument('-D', "--debug", help="build debug target", action='store_true')
parser.add_argument('-j', "--threads", help="number of threads to run", default=10, type=int)
parser.add_argument('-v', "--verbose", help="print compiler calls", action='store_true')
parser.add_argument("-T", "--target", help="target controller to build", default="", nargs='*')
args = parser.parse_args()

IS_CLEANUP = args.clean

#if IS_CLEANUP:
if os.path.exists(OUTPUT_PATH):
    for root, dirs, files in os.walk(OUTPUT_PATH, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))
#    sys.exit(0)

TargetConfig = namedtuple('TargetConfig', [
    'target',
    'sourcefiles', 'sourcedirs',
    'cflags', 'asmflags', 'ldflags', 'useColor'
])

def configure_target(TARGET):
    # required features
    FEATURES = []

    # stm32f051x8 stm32f103xb stm32f303xc stm32f405xx stm32f411xe stm32f745xx stm32f746xx

    ################################################################################
    # Determine target variables and features

    if TARGET == "stm32f051x8":
        PROJECT = "esc"
        TARGET_DEVICE = "STM32F051x8"
        TARGET_SCRIPT = "stm32_flash_f051_32k.ld"
        TARGET_PROCESSOR_TYPE  = "f0"
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f103xb":
        PROJECT = "flight_controller"
        TARGET_DEVICE = "STM32F103xB"
        TARGET_SCRIPT = "stm32_flash_f103_128k.ld"
        TARGET_PROCESSOR_TYPE  = "f1"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f303xc":
        PROJECT = "flight_controller"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_256k_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f405xx":
        PROJECT = "flight_controller"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f405xx_rfbl":
        PROJECT = "boot_loader"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405_rfbl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Og"

    elif TARGET == "stm32f405xx_rfbll":
        PROJECT = "recovery_loader"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405_recovery.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Og"

    elif TARGET == "stm32f411xe":
        PROJECT = "flight_controller"
        TARGET_DEVICE = "STM32F411xE"
        TARGET_SCRIPT = "stm32_flash_f411.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f746xx":
        PROJECT = "flight_controller"
        TARGET_DEVICE = "STM32F746xx"
        TARGET_SCRIPT = "STM32F746NGHx_FLASH_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f7"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f446xx":
        PROJECT = "flight_controller"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O3"

    elif TARGET == "stm32f446xx_rfbl":
        PROJECT = "boot_loader"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446_rfbl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "stm32f446xx_rfbll":
        PROJECT = "recovery_loader"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446_recovery.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Og"
    
    else:
        print("ERROR: NOT VALID TARGET: ", TARGET, file=sys.stderr)
        sys.exit(1)


        """
    if TARGET == "cc3d":
        PROJECT = "rffw"
        TARGET_BOARD = "cc3d"
        TARGET_DEVICE = "STM32F103xB"
        TARGET_SCRIPT = "stm32_flash_f103_128k.ld"
        TARGET_PROCESSOR_TYPE  = "f1"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "cc3d_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "cc3d"
        TARGET_DEVICE = "STM32F103xB"
        TARGET_SCRIPT = "stm32_flash_f103_128k.ld"
        TARGET_PROCESSOR_TYPE  = "f1"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "sp3evo":
        PROJECT = "rffw"
        TARGET_BOARD = "sp3evo"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_256k_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["flight_logger", "mpu6500/spi", "usb_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "sp3evo_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "sp3evo"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_256k.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "lux":
        PROJECT = "rffw"
        TARGET_BOARD = "lux"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_256k_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["mpu6500/spi", "usb_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "lux_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "lux"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_256k.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "colibri":
        PROJECT = "rffw"
        TARGET_BOARD = "colibri"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_128k.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["mpu6500/spi", "usb_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "colibri_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "colibri"
        TARGET_DEVICE = "STM32F303xC"
        TARGET_SCRIPT = "stm32_flash_f303_128k.ld"
        TARGET_PROCESSOR_TYPE  = "f3"
        FEATURES.extend(["usb_fs"])
        OPTIMIZE_FLAGS = "-O2"
        
    elif TARGET == "revolt":
        PROJECT = "rffw"
        TARGET_BOARD = "revolt"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["mpu6500/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "revolt_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "revolt"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "revo":
        PROJECT = "rffw"
        TARGET_BOARD = "revo"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["flight_logger", "mpu6000/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "revo_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "revo"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"
        
    elif TARGET == "kkng":
        PROJECT = "rffw"
        TARGET_BOARD = "kkng"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["mpu6000/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "kkng_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "kkng"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "stm32_flash_f405.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"
        
    elif TARGET == "revonano":
        PROJECT = "rffw"
        TARGET_BOARD = "revonano"
        TARGET_DEVICE = "STM32F411xE"
        TARGET_SCRIPT = "stm32_flash_f411.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["mpu6000/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "revonano_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "revonano"
        TARGET_DEVICE = "STM32F411xE"
        TARGET_SCRIPT = "stm32_flash_f411.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "spmfc384":
        PROJECT = "rffw"
        TARGET_BOARD = "spmfc384"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["mpu6500/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "spmfc384_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "spmfc384"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "spmfc400":
        PROJECT = "rffw"
        TARGET_BOARD = "spmfc400"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["mpugyro/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "spmfc400_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "spmfc400"
        TARGET_DEVICE = "STM32F446xx"
        TARGET_SCRIPT = "stm32_flash_f446.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "f7disco":
        PROJECT = "rffw"
        TARGET_BOARD = "f7disco"
        TARGET_DEVICE = "STM32F746xx"
        TARGET_SCRIPT = "STM32F746NGHx_FLASH_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f7"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "f7disco_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "f7disco"
        TARGET_DEVICE = "STM32F746xx"
        TARGET_SCRIPT = "STM32F746NGHx_FLASH.ld"
        TARGET_PROCESSOR_TYPE  = "f7"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "vrracef7":
        PROJECT = "rffw"
        TARGET_BOARD = "vrracef7"
        TARGET_DEVICE = "STM32F746xx"
        TARGET_SCRIPT = "STM32F746NGHx_FLASH_bl.ld"
        TARGET_PROCESSOR_TYPE  = "f7"
        FEATURES.extend(["mpu6500/spi", "usb_otg_fs"])
        OPTIMIZE_FLAGS = "-O2"

    elif TARGET == "vrracef7_rfbl":
        PROJECT = "rfbl"
        TARGET_BOARD = "revolt"
        TARGET_DEVICE = "STM32F405xx"
        TARGET_SCRIPT = "STM32F746NGHx_FLASH.ld"
        TARGET_PROCESSOR_TYPE  = "f4"
        FEATURES.extend(["usb_otg_fs"])
        OPTIMIZE_FLAGS = "-Os"

    elif TARGET == "kissesc":
        PROJECT = "rfesc"
        TARGET_BOARD = "kissesc"
        TARGET_DEVICE = "STM32F051x8"
        TARGET_SCRIPT = "stm32_flash_f051_32k.ld"
        TARGET_PROCESSOR_TYPE  = "f0"
        OPTIMIZE_FLAGS = "-O2"
    """



    ################################################################################
    # Set per target compilation options

    STM32F0_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE + " -DARM_MATH_CM0 -D" + TARGET
    STM32F0_ARCH_FLAGS = "-mthumb -mcpu=cortex-m0"

    STM32F1_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE + " -DARM_MATH_CM3 -D" + TARGET
    STM32F1_ARCH_FLAGS = "-mthumb -mcpu=cortex-m3"

    STM32F3_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE + " -DARM_MATH_CM4 -D" + TARGET
    STM32F3_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"

    if TARGET_DEVICE == "STM32F446xx":
        STM32F4_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=12000000 -D" + TARGET_DEVICE + " -DARM_MATH_CM4 -D" + TARGET
        STM32F4_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"
    else:
        STM32F4_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE + " -DARM_MATH_CM4 -D" + TARGET
        STM32F4_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"

    STM32F7_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=25000000 -D" + TARGET_DEVICE + " -DARM_MATH_CM7 -D" + TARGET
    STM32F7_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"


    if TARGET_PROCESSOR_TYPE == "f0":
        DEF_FLAGS  = STM32F0_DEF_FLAGS
        ARCH_FLAGS = STM32F0_ARCH_FLAGS
        MCU_FAMILY = "stm32"

    elif TARGET_PROCESSOR_TYPE == "f1":
        DEF_FLAGS  = STM32F1_DEF_FLAGS
        ARCH_FLAGS = STM32F1_ARCH_FLAGS
        MCU_FAMILY = "stm32"

    elif TARGET_PROCESSOR_TYPE == "f3":
        DEF_FLAGS  = STM32F3_DEF_FLAGS
        ARCH_FLAGS = STM32F3_ARCH_FLAGS
        MCU_FAMILY = "stm32"

    elif TARGET_PROCESSOR_TYPE == "f4":
        DEF_FLAGS  = STM32F4_DEF_FLAGS
        ARCH_FLAGS = STM32F4_ARCH_FLAGS
        MCU_FAMILY = "stm32"

    elif TARGET_PROCESSOR_TYPE == "f7":
        DEF_FLAGS  = STM32F7_DEF_FLAGS
        ARCH_FLAGS = STM32F7_ARCH_FLAGS
        MCU_FAMILY = "stm32"

    else:
        print("ERROR: NOT VALID PROCESSOR TYPE FOR TARGET %s, CHECK MAKE FILE CODE" % TARGET, file=sys.stderr)
        sys.exit(1)

    MCU_DIR    = "src/low_level_driver/stm32%s" % TARGET_PROCESSOR_TYPE
    CMSIS_DIR  = "lib/CMSIS/Device/ST/STM32%sxx/Include" % TARGET_PROCESSOR_TYPE.upper()
    HAL_DIR    = "lib/STM32%sxx_HAL_Driver" % TARGET_PROCESSOR_TYPE.upper()


    ################################################################################
    # Set source and includes directories

    # common directories

    INCLUDE_DIRS = [
        "lib/CMSIS/Include",
        "lib/CMSIS/DSP_Lib/Include",
        "src/%s/inc" % PROJECT,
        CMSIS_DIR,
        HAL_DIR + "/Inc",
        MCU_DIR,
        "src/low_level_driver/" + MCU_FAMILY,
    ]

    SOURCE_DIRS = [
        HAL_DIR + "/Src",
        "src/%s/src" % PROJECT,
        MCU_DIR,
        "src/low_level_driver/",
        "src/low_level_driver/" + MCU_FAMILY,
    ]

    SOURCE_FILES = ["src/low_level_driver/stm32_startup/startup_%s.s" % TARGET_DEVICE.lower()]

    # per project includes

    if PROJECT == "flight_controller":
        INCLUDE_DIRS.append("src/flight_controller/inc/input")
        SOURCE_DIRS.append("src/flight_controller/src/input")
        FEATURES.extend(["esc_1wire", "leds", "dmaShenanigans", "actuator_output", "buzzer", "flash_chip", "mpu_icm_device/spi", "rx", "serial", "spektrumTelemetry"])
    elif PROJECT == "esc":
        FEATURES.extend(["leds"])
    elif PROJECT == "boot_loader":
        FEATURES.extend(["leds"])
    elif PROJECT == "recovery_loader":
        FEATURES.extend(["leds"])
    else:
        print("ERROR: NOT VALID PROJECT TYPE, CHECK MAKE FILE CODE", file=sys.stderr)
        sys.exit(1)

    # per-feature directories and files

    USB_SOURCE_DIRS = [
        "lib/STM32_USB_Device_Library/Core/Src",
        "lib/STM32_USB_Device_Library/Class/HID/Src",
        "src/%s/src/usb" % PROJECT,
    ]

    USB_INCLUDE_DIRS = [
        "lib/STM32_USB_Device_Library/Core/Inc",
        "lib/STM32_USB_Device_Library/Class/HID/Inc",
        "src/%s/inc/usb" % PROJECT,
    ]

    for feature in FEATURES:

        if feature.startswith("usb_"):
            # add common usb directories and usb descriptor for project
            SOURCE_DIRS.extend(USB_SOURCE_DIRS)
            INCLUDE_DIRS.extend(USB_INCLUDE_DIRS)
            # add usb class specific files
            SOURCE_DIRS.append("src/low_level_driver/" + feature)
            INCLUDE_DIRS.append("src/low_level_driver/" + feature)

        elif feature.startswith("mpu"):
            # gyro named by "gyro/bus", e.g. "mpu6000/spi"
            gyro, bus = feature.split("/")
            SOURCE_FILES.append("src/%s/src/drivers/invensense_%s.c" % (PROJECT, gyro))
            SOURCE_FILES.append("src/%s/src/drivers/invensense_bus_%s.c" % (PROJECT, bus))
            INCLUDE_DIRS.append("src/%s/inc/drivers" % PROJECT)

        else:
            SOURCE_FILES.append("src/%s/src/drivers/" % (PROJECT) + feature + ".c")
            INCLUDE_DIRS.append("src/%s/inc/drivers/" % (PROJECT))

    SOURCE_FILES.append("lib/CMSIS/DSP_Lib/Source/FilteringFunctions/arm_fir_init_f32.c")
    SOURCE_FILES.append("lib/CMSIS/DSP_Lib/Source/FilteringFunctions/arm_fir_f32.c")
    ################################################################################
    # compiler options

    INCLUDES = " ".join("-I" + include for include in INCLUDE_DIRS)

    LTO_FLAGS = "-flto -fuse-linker-plugin"
    DEBUG_FLAGS = "-ggdb3 -DDEBUG -Og"

    CFLAGS = " ".join([
        ARCH_FLAGS,
        LTO_FLAGS,
        DEF_FLAGS,
        DEBUG_FLAGS if args.debug else OPTIMIZE_FLAGS,
        INCLUDES,
        "-Wall -Wextra -Wmaybe-uninitialized -fno-unsafe-math-optimizations -Wdouble-promotion "
        "-ffunction-sections -fdata-sections -MMD -MP"
    ])

    ASMFLAGS = " ".join([
        ARCH_FLAGS,
        "-x assembler-with-cpp",
        INCLUDES,
        "-MMD -MP"
    ])

    mapFile = os.path.join("output", TARGET + ".map")
    linkerDir = os.path.join("src", "low_level_driver")
    ldScript = os.path.join("src", "low_level_driver", TARGET_SCRIPT)
    LDFLAGS = " ".join([
        "-lm -nostartfiles --specs=nano.specs -lc -lnosys",
        ARCH_FLAGS,
        LTO_FLAGS,
        DEBUG_FLAGS if args.debug else OPTIMIZE_FLAGS,
        "-static",
        "-Wl,-gc-sections,-Map," + mapFile,
        "-Wl,-L" + linkerDir,
        "-Wl,--cref",
        "-T" + ldScript
    ])

    # if we're at a tty, then tell gcc to use colors
    if sys.stdout.isatty():
        colorFlag = "-fdiagnostics-color"
    else:
        colorFlag = ""

    ################################################################################
    # build return object with all needed parameters

    target_config = TargetConfig(
        target=TARGET,
        sourcefiles=SOURCE_FILES,
        sourcedirs=SOURCE_DIRS,
        cflags=CFLAGS,
        asmflags=ASMFLAGS,
        ldflags=LDFLAGS,
        useColor=colorFlag,
    )

    return target_config



asm_command = "arm-none-eabi-gcc -c {USECOLOR} -o output/{OUTPUT_FILE} {ASMFLAGS} {INPUT_FILE}"

compile_command = "arm-none-eabi-gcc -c {USECOLOR} -o output/{OUTPUT_FILE} {CFLAGS} {INPUT_FILE}"

link_command = "arm-none-eabi-gcc {USECOLOR} -o output/{OUTPUT_NAME}.elf {OBJS} {LDFLAGS}"

size_command = "arm-none-eabi-size output/{OUTPUT_NAME}.elf"

copy_obj_command = "arm-none-eabi-objcopy -O binary output/{OUTPUT_NAME}.elf output/{OUTPUT_NAME}.bin"


excluded_files = [
    ".*_template.c",
]


THREAD_LIMIT = args.threads
threadLimiter = threading.BoundedSemaphore(THREAD_LIMIT)
locker = threading.Lock()
threadRunning = list()
isStop = False

def find_between( s, first, last ):
    try:
        start = s.index( first )
        end = s.index( last, start )
        return s[start:end]
    except ValueError:
        return ""

class CommandRunnerThread(threading.Thread):

    def __init__(self, command, output, target, *args, **kwargs):
        self.command = command.path  # we need to be sure all '/' are properly converted for Windows
        self.output = output.path    # store the output of the command for printing purposes
        self.target = target
        self.queue = kwargs.pop("queue", None)
        self.deps = kwargs.pop("dependencies", None)
        self.proc = None
        super(CommandRunnerThread, self).__init__(*args, **kwargs)
        self.stop_event = threading.Event()

    def run(self):
        if self.deps:
            while self.deps:
                if isStop:
                    return
                # wrap in try, in case dependency threads haven't started yet
                try:
                    # wait for first thread to be done
                    self.deps[0].join()
                    # it's done, pop it off deps
                    self.deps.pop(0)
                except RuntimeError:
                    pass

        with threadLimiter:
            with locker:
                threadRunning.append(self)

            try:
                self.run_command()
            finally:
                with locker:
                    threadRunning.remove(self)

    def run_command(self):
        if not self.command:
            return

        with locker:
            if isStop:
                return

            # figure out the output file path
            basedir, basename = os.path.split(self.output)
            _, ext = os.path.splitext(basename)
            # if the base directory doesn't exist, make it
            if not os.path.exists(basedir):
                os.makedirs(basedir)

        self.proc = subprocess.Popen(self.command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout_value, stderr_value = self.proc.communicate()

        # "arm-none-eabi-XXXX" -> "XXXX"
        executable = self.command.split(" ")[0]
        toolChainCmd = executable.split("-")[-1]

        with locker:
            # output all .c/.s file outputs with green, others with red
            if ext == ".o":
                foreColor = Fore.GREEN
            else:
                foreColor = Fore.RED
            print(Fore.MAGENTA + "%% {:s} ".format(self.target) + foreColor + toolChainCmd + " " + Style.RESET_ALL + basename)

            if (args.verbose):
                print(self.command)

            if stdout_value:
                print(stdout_value.decode())
            if stderr_value:
                print(stderr_value.decode())

            sys.stdout.flush()

        if self.queue:
            self.queue.put(self.proc.returncode)
        else:
            print(self.proc.returncode)

        self.proc = None

    def stop_command(self):
        if self.proc:
            try:
                self.proc.kill()
                self.proc.wait()
            except OSError:
                pass

        self.stop_event.set()


    def stopped(self):
        return self.stop_event.isSet()


def FileModified(fileName, target_config):
    # get the output file `output/.../filename.o`
    outputFile = os.path.join("output", makeObject(fileName.path, target_config.target))
    # if we haven't compiled, then return true
    if not os.path.exists(outputFile):
        return True

    # if input file is more recent than the output, return true
    if os.path.getmtime(fileName) > os.path.getmtime(outputFile):
        return True

    # if the target file is more recent than the output, return true
    target_file = "src/low_level_driver/boarddef.h".path
    if os.path.getmtime(target_file) > os.path.getmtime(outputFile):
        return True

    # get the dependency file `output/.../filename.d`
    outputBase, _ = os.path.splitext(outputFile)
    depFile = outputBase + ".d"

    # if we don't have a dependency file, return true
    if not os.path.exists(depFile):
        return True

    # check the dependency file
    with open(depFile, 'r') as f:
        for line in f:
            # the lines with dependencies start with a space
            if line[0] != " ":
                continue

            for dep in line.split():
                # we'll get the line continuation in this: "\"
                if dep == "\\":
                    continue
                # all dependencies should exist
                if not os.path.exists(dep):
                    return True
                # check if dependency is more recent
                if os.path.getmtime(dep) > os.path.getmtime(outputFile):
                    return True

    return False

def makeObject(fileName, target_dir):
    head, tail = os.path.split(fileName)
    root, ext = os.path.splitext(tail)
    if ext.lower() in (".c", ".s"):
        root = os.path.join(target_dir, root)
        return root + ".o"

    print("Unknown file type: " + tail)
    return tail

#This works, but the linker doesn't know how to find the files
#    root, ext = os.path.splitext(fileName)
#
#    # strip first directory from  "src/..." or "lib/..."
#    _, root = root.split(os.sep, 1)
#    # send it to "output/target/"
#    root = os.path.join(TARGET_BOARD, root)
#
#    if ext.lower() in (".c", ".s"):
#        return root + ".o"
#
#    print("Unknown file type: " + tail)
#    return root

def ProcessList(fileNames, target_config):
    linkerObjs = []
    commands = []

    for fileName in fileNames:
        if any(re.match(ex_pattern, os.path.basename(fileName)) for ex_pattern in excluded_files):
            continue

        linkerObjs.append(os.path.join("output", makeObject(fileName, target_config.target)))
        if FileModified(fileName, target_config):
            if fileName[-2:] == ".s":
                commands.append(asm_command.format(
                    INPUT_FILE=fileName.path,
                    OUTPUT_FILE=makeObject(fileName.path, target_config.target),
                    ASMFLAGS=target_config.asmflags,
                    USECOLOR=target_config.useColor,
                ))
            elif fileName[-2:] == ".c":
                commands.append(compile_command.format(
                    INPUT_FILE=fileName.path,
                    OUTPUT_FILE=makeObject(fileName.path, target_config.target),
                    CFLAGS=target_config.cflags,
                    USECOLOR=target_config.useColor,
                ))
            else:
                raise Exception("Bad file type:", fileName)
    return commands, linkerObjs


def main():
    global isStop

    if not args.target:
        raise Exception("Output target must be specified!")

    try:
        os.mkdir("output")
    except:
        pass

    threads = []
    thread_queue = queue.Queue()

    for target in args.target:
        target_config = configure_target(target)

        commands = []
        linkerObjs = []

        # parse all directories for .c and .s files
        for directory in target_config.sourcedirs:
            # process each file, add commands and output files to list
            command, linkerObj = ProcessList(glob.glob(os.path.join(directory, "*.c")), target_config)
            commands.extend(command)
            linkerObjs.extend(linkerObj)

            command, linkerObj = ProcessList(glob.glob(os.path.join(directory, "*.s")), target_config)
            commands.extend(command)
            linkerObjs.extend(linkerObj)

        command, linkerObj = ProcessList(target_config.sourcefiles, target_config)
        commands.extend(command)
        linkerObjs.extend(linkerObj)

        # generate list of threads from all commands
        linkerThreads = []
        for command, linkerObj in zip(commands, linkerObjs):
            thread = CommandRunnerThread(command=command, output=linkerObj, target=target_config.target, queue=thread_queue)
            threads.append(thread)
            linkerThreads.append(thread)

        linkTarget = link_command.format(
            OUTPUT_NAME=target_config.target,
            OBJS=" ".join(linkerObjs),
            LDFLAGS=target_config.ldflags,
            USECOLOR=target_config.useColor,
        )
        linkOutput = "output/{OUTPUT_NAME}.elf".format(OUTPUT_NAME=target_config.target)
        linkThread = CommandRunnerThread(command=linkTarget, output=linkOutput, target=target_config.target, queue=thread_queue, dependencies=linkerThreads)
        threads.append(linkThread)

        sizeTarget = size_command.format(
            OUTPUT_NAME=target_config.target,
        )
        sizeOutput = "output/{OUTPUT_NAME}.elf".format(OUTPUT_NAME=target_config.target)
        sizeThread = CommandRunnerThread(command=sizeTarget, output=sizeOutput, target=target_config.target, queue=thread_queue, dependencies=[linkThread])
        threads.append(sizeThread)

        copyTarget = copy_obj_command.format(
            OUTPUT_NAME=target_config.target
        )
        copyOutput = "output/{OUTPUT_NAME}.bin".format(OUTPUT_NAME=target_config.target)
        copyThread = CommandRunnerThread(command=copyTarget, output=copyOutput, target=target_config.target, queue=thread_queue, dependencies=[sizeThread])
        threads.append(copyThread)

    # all threads are created, start them up
    for thread in threads:
        thread.start()

    while len(threadRunning) > 0:
        try:
            returncode = thread_queue.get(timeout=5)
        except queue.Empty:
            continue
        if returncode > 0:
            with locker:
                isStop = True
                for thread in threads:
                    thread.stop_command()
            break

    for thread in threads:
        thread.join()


if __name__ == "__main__":
    main()







