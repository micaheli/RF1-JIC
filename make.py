#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

try:
    # Python 3 name
    import queue
except ImportError:
    # Python 2 name
    import Queue as queue


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



parser = argparse.ArgumentParser(description='')
parser.add_argument("-T", "--target", help="target controller to build", default="")
parser.add_argument('-C', "--clean", help="clean up output folder", action='store_true')
parser.add_argument('-D', "--debug", help="build debug target", action='store_true')
parser.add_argument('-j', "--threads", help="number of threads to run", default=10, type=int)
args = parser.parse_args()

TARGET = args.target.lower()
IS_CLEANUP = args.clean

# TODO: replace all output path to global path
OUTPUT_PATH = "output"

if IS_CLEANUP:
    if os.path.exists(OUTPUT_PATH):
        for root, dirs, files in os.walk(OUTPUT_PATH, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
    sys.exit(0)

FEATURES = []

################################################################################
# Determine target variables and features

if TARGET == "cc3d":
    PROJECT = "rffw"
    TARGET_BOARD = "cc3d"
    TARGET_DEVICE = "STM32F103xB"
    TARGET_SCRIPT = "stm32_flash_f103_128k.ld"
    TARGET_PROCESSOR_TYPE  = "f1"
    FEATURES = ["usb_fs"]
    OPTIMIZE_FLAGS = "-O2"

elif TARGET == "cc3d_rfbl":
    PROJECT = "rfbl"
    TARGET_BOARD = "cc3d"
    TARGET_DEVICE = "STM32F103xB"
    TARGET_SCRIPT = "stm32_flash_f103_128k.ld"
    TARGET_PROCESSOR_TYPE  = "f1"
    FEATURES = ["leds", "usb_fs"]
    OPTIMIZE_FLAGS = "-Os"

elif TARGET == "kissesc":
    PROJECT = "rfesc"
    TARGET_BOARD = "kissesc"
    TARGET_DEVICE = "STM32F051x8"
    TARGET_SCRIPT = "stm32_flash_f051_32k.ld"
    TARGET_PROCESSOR_TYPE  = "f0"
    OPTIMIZE_FLAGS = "-O2"

elif TARGET == "lux":
    PROJECT = "rffw"
    TARGET_BOARD = "lux"
    TARGET_DEVICE = "STM32F303xC"
    TARGET_SCRIPT = "stm32_flash_f303_128k.ld"
    TARGET_PROCESSOR_TYPE  = "f3"
    FEATURES = ["mpu6500/spi", "usb_fs"]
    OPTIMIZE_FLAGS = "-O2"

elif TARGET == "lux_rfbl":
    PROJECT = "rfbl"
    TARGET_BOARD = "lux"
    TARGET_DEVICE = "STM32F303xC"
    TARGET_SCRIPT = "stm32_flash_f303_128k.ld"
    TARGET_PROCESSOR_TYPE  = "f3"
    FEATURES = ["leds", "usb_fs"]
    OPTIMIZE_FLAGS = "-O2"

elif TARGET == "revo":
    PROJECT = "rffw"
    TARGET_BOARD = "revo"
    TARGET_DEVICE = "STM32F405xx"
    TARGET_SCRIPT = "stm32_flash_f405.ld"
    TARGET_PROCESSOR_TYPE  = "f4"
    FEATURES = ["mpu6000/spi", "usb_otg_fs"]
    OPTIMIZE_FLAGS = "-O2"

elif TARGET == "revo_rfbl":
    PROJECT = "rfbl"
    TARGET_BOARD = "revo"
    TARGET_DEVICE = "STM32F405xx"
    TARGET_SCRIPT = "stm32_flash_f405.ld"
    TARGET_PROCESSOR_TYPE  = "f4"
    FEATURES = ["leds", "usb_otg_fs"]
    OPTIMIZE_FLAGS = "-Os"

elif TARGET == "f7disco":
    PROJECT = "rffw"
    TARGET_BOARD = "f7disco"
    TARGET_DEVICE = "STM32F746xx"
    TARGET_SCRIPT = "STM32F746NGHx_FLASH.ld"
    TARGET_PROCESSOR_TYPE  = "f7"
    FEATURES = ["usb_otg_fs"]
    OPTIMIZE_FLAGS = "-O2"

else:
    print("ERROR: NOT VALID TARGET", file=sys.stderr)
    sys.exit(1)


################################################################################
# Set per target compilation options

STM32F0_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F0_ARCH_FLAGS = "-mthumb -mcpu=cortex-m0"

STM32F1_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F1_ARCH_FLAGS = "-mthumb -mcpu=cortex-m3"

STM32F3_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F3_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"

STM32F4_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F4_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"

STM32F7_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=25000000 -D" + TARGET_DEVICE
STM32F7_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"


if TARGET_PROCESSOR_TYPE == "f0":
    DEF_FLAGS  = STM32F0_DEF_FLAGS
    ARCH_FLAGS = STM32F0_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f1":
    DEF_FLAGS  = STM32F1_DEF_FLAGS
    ARCH_FLAGS = STM32F1_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f3":
    DEF_FLAGS  = STM32F3_DEF_FLAGS
    ARCH_FLAGS = STM32F3_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f4":
    DEF_FLAGS  = STM32F4_DEF_FLAGS
    ARCH_FLAGS = STM32F4_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f7":
    DEF_FLAGS  = STM32F7_DEF_FLAGS
    ARCH_FLAGS = STM32F7_ARCH_FLAGS

else:
    print("ERROR: NOT VALID PROCESSOR TYPE, CHECK MAKE FILE CODE", file=sys.stderr)
    sys.exit(1)

MCU_DIR    = "src/target/stm32%s" % TARGET_PROCESSOR_TYPE
CMSIS_DIR  = "lib/CMSIS/Device/ST/STM32%sxx/Include" % TARGET_PROCESSOR_TYPE.upper()
HAL_DIR    = "lib/STM32%sxx_HAL_Driver" % TARGET_PROCESSOR_TYPE.upper()


################################################################################
# Set source and includes directories

# common directories

INCLUDE_DIRS = [
    "lib/CMSIS/Include",
    "src/%s/inc" % PROJECT,
    CMSIS_DIR,
    HAL_DIR + "/Inc",
    "src/target/" + TARGET_BOARD,
    MCU_DIR,
]

SOURCE_DIRS = [
    HAL_DIR + "/Src",
    "src/%s/src" % PROJECT,
    "src/target/" + TARGET_BOARD,
    MCU_DIR,
]

SOURCE_FILES = []

# per project includes

if PROJECT == "rffw":
    INCLUDE_DIRS.append("src/rffw/inc/input")
    SOURCE_DIRS.append("src/rffw/src/input")
elif PROJECT == "rfesc":
    pass
elif PROJECT == "rfbl":
    pass
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
        SOURCE_DIRS.append("src/target/" + feature)
        INCLUDE_DIRS.append("src/target/" + feature)

    elif feature.startswith("mpu"):
        # gyro named by "gyro/bus", e.g. "mpu6000/spi"
        gyro, bus = feature.split("/")
        SOURCE_FILES.append("src/%s/src/drivers/invensense_%s.c" % (PROJECT, gyro))
        SOURCE_FILES.append("src/%s/src/drivers/invensense_bus_%s.c" % (PROJECT, bus))
        INCLUDE_DIRS.append("src/%s/inc/drivers" % PROJECT)

    else:
        SOURCE_FILES.append("src/%s/src/drivers/" % (PROJECT) + feature + ".c")
        INCLUDE_DIRS.append("src/%s/inc/drivers/" % (PROJECT))


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
    "-Wunused-parameter -Wdouble-promotion -save-temps=obj -std=gnu99",
    "-Wall -Wextra -Wunsafe-loop-optimizations",
    "-ffunction-sections -fdata-sections -MMD -MP"
])

ASMFLAGS = " ".join([
    ARCH_FLAGS,
    "-x assembler-with-cpp",
    INCLUDES,
    "-MMD -MP"
])

mapFile = os.path.join("output", "{OUTPUT_NAME}.map")
linkerDir = os.path.join("src", "target")
ldScript = os.path.join("src", "target", TARGET_SCRIPT)
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

asm_command = "arm-none-eabi-gcc -c -fdiagnostics-color -o output" + os.sep + "{OUTPUT_FILE} " + ASMFLAGS + " {INPUT_FILE}"

compile_command = "arm-none-eabi-gcc -c -fdiagnostics-color -o output" + os.sep + "{OUTPUT_FILE} " + CFLAGS + " {INPUT_FILE}"

link_command = "arm-none-eabi-gcc -fdiagnostics-color -o output" + os.sep + "{OUTPUT_NAME}.elf {OBJS} " + LDFLAGS

size_command = "arm-none-eabi-size output" + os.sep + "{OUTPUT_NAME}.elf"

copy_obj_command = "arm-none-eabi-objcopy -O binary output" + os.sep + "{OUTPUT_NAME}.elf output" + os.sep + "{OUTPUT_NAME}.bin"


commands = []
linkerObjs = ""

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

    def __init__(self, *args, **kwargs):
        self.command = kwargs.pop("command", "")
        self.queue = kwargs.pop("queue", None)
        self.proc = None
        super(CommandRunnerThread, self).__init__(*args, **kwargs)
        self.stop_event = threading.Event()


    def run(self):
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
            file_path = find_between(self.command, "output" + os.sep, ".o")
            basedir, basename = os.path.split(file_path)
            # if the base directory doesn't exist, make it
            if not os.path.exists(basedir):
                os.makedirs(basedir)

        self.proc = subprocess.Popen(self.command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout_value, stderr_value = self.proc.communicate()

        with locker:
            print("% " + basename)
            if stderr_value:
                print(stderr_value.decode())

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


def FileModified(fileName):
    # get the output file `output/.../filename.o`
    outputFile = os.path.join("output", makeObject(fileName.path))
    # if we haven't compiled, then return true
    if not os.path.exists(outputFile):
        return True

    # if input file is more recent than the output, return true
    if os.path.getmtime(fileName) > os.path.getmtime(outputFile):
        return True

    # if the target file is more recent than the output, return true
    target_file = "src/target/%s/target.h".path % TARGET_BOARD
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

def AddToList(fileName):
    global commands

    commands.append(fileName)

def makeObject(fileName):
    head, tail = ntpath.split(fileName)
    root, ext = os.path.splitext(tail)
    if ext.lower() in (".c", ".s"):
        root = os.path.join(TARGET, root)
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

def ProcessList(fileNames):
    global linkerObjs

    for fileName in fileNames:
        if any(re.match(ex_pattern, os.path.basename(fileName)) for ex_pattern in excluded_files):
            continue

        linkerObjs = linkerObjs + " " + os.path.join("output", makeObject(fileName))
        if FileModified(fileName):
            if fileName[-2:] == ".s":
                AddToList(asm_command.format(INPUT_FILE=fileName.path, OUTPUT_FILE=makeObject(fileName.path)))
            elif fileName[-2:] == ".c":
                AddToList(compile_command.format(INPUT_FILE=fileName.path, OUTPUT_FILE=makeObject(fileName.path)))
            else:
                AddToList(asm_command.format(INPUT_FILE=fileName.path, OUTPUT_FILE=makeObject(fileName.path)))


def main():
    global size_command, link_command, isStop, copy_obj_command

    try:
        os.mkdir("output")
    except:
        pass

    for directory in SOURCE_DIRS:
        ProcessList(glob.glob(os.path.join(directory, "*.c")))

    ProcessList(SOURCE_FILES)

    for directory in SOURCE_DIRS:
        ProcessList(glob.glob(os.path.join(directory, "*.s")))

    thread_queue = queue.Queue()
    threads = list()
    for command in commands:
        thread = CommandRunnerThread(command=command, queue=thread_queue)
        threads.append(thread)

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

    if isStop is False:
        print("linking...")
        link_command = link_command.format(
            OUTPUT_NAME=TARGET,
            OBJS=linkerObjs
        )

        proc = subprocess.Popen(link_command, shell=True)
        stdout_value, stderr_value = proc.communicate()

        if proc.returncode:
            sys.exit(proc.returncode)


        print("Sizing!")
        size_command = size_command.format(
            OUTPUT_NAME=TARGET
        )

        print(size_command)
        proc = subprocess.Popen(size_command, shell=True)
        stdout_value, stderr_value = proc.communicate()

        if proc.returncode:
            sys.exit(proc.returncode)


        print("Build succeded copying")
        copy_obj_command = copy_obj_command.format(
            OUTPUT_NAME=TARGET
        )

        print(copy_obj_command)
        proc = subprocess.Popen(copy_obj_command, shell=True)
        stdout_value, stderr_value = proc.communicate()

        if proc.returncode:
            sys.exit(proc.returncode)
     





if __name__ == "__main__":
    main()







