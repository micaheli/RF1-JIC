#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import sys
import threading
import Queue
import subprocess
import glob
import sys
import ntpath
import os
import platform
import argparse
import ctypes as c



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
parser.add_argument('-C', "--clean", help="clean up output folder", action='store_const', const=True, default=False)
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


if TARGET == "cc3d":
    TARGET_DEVICE = "STM32F103xB"
    TARGET_SCRIPT = "stm32_flash_f103_128k.ld"
    TARGET_PROCESSOR_TYPE  = "f1"

elif TARGET == "lux":
    TARGET_DEVICE = "STM32F303xE"
    TARGET_SCRIPT = "stm32_flash_f303_128k.ld"
    TARGET_PROCESSOR_TYPE  = "f3"

elif TARGET == "revo":
    TARGET_DEVICE = "STM32F405xx"
    TARGET_SCRIPT = "stm32_flash_f405.ld"
    TARGET_PROCESSOR_TYPE  = "f4"

elif TARGET == "f7disco":
    TARGET_DEVICE = "STM32F746xx"
    TARGET_SCRIPT = "STM32F746NGHx_FLASH.ld"
    TARGET_PROCESSOR_TYPE  = "f7"

else:
    print("ERROR: NOT VALID TARGET")
    sys.exit(1)



STM32F1_MCU_DIR    = "src/rffw/target/stm32f1"
STM32F1_CMSIS_DIR  = "lib/CMSIS/Device/ST/STM32F1xx/Include"
STM32F1_HAL_DIR    = "lib/STM32F1xx_HAL_Driver"
STM32F1_HAL_SRC    = "lib/STM32F1xx_HAL_Driver"
STM32F1_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F1_ARCH_FLAGS = "-mthumb -mcpu=cortex-m3"

STM32F3_MCU_DIR    = "src/rffw/target/stm32f3"
STM32F3_CMSIS_DIR  = "lib/CMSIS/Device/ST/STM32F3xx/Include"
STM32F3_HAL_DIR    = "lib/STM32F3xx_HAL_Driver"
STM32F3_HAL_SRC    = "lib/STM32F3xx_HAL_Driver"
STM32F3_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F3_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"

STM32F4_MCU_DIR    = "src/rffw/target/stm32f4"
STM32F4_CMSIS_DIR  = "lib/CMSIS/Device/ST/STM32F4xx/Include"
STM32F4_HAL_DIR    = "lib/STM32F4xx_HAL_Driver"
STM32F4_HAL_SRC    = "lib/STM32F4xx_HAL_Driver"
STM32F4_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -D" + TARGET_DEVICE
STM32F4_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"

STM32F7_MCU_DIR    = "src/rffw/target/stm32f7"
STM32F7_CMSIS_DIR  = "lib/CMSIS/Device/ST/STM32F7xx/Include"
STM32F7_HAL_DIR    = "lib/STM32F7xx_HAL_Driver"
STM32F7_HAL_SRC    = "lib/STM32F7xx_HAL_Driver"
STM32F7_DEF_FLAGS  = "-DUSE_HAL_DRIVER -DHSE_VALUE=25000000 -D" + TARGET_DEVICE
STM32F7_ARCH_FLAGS = "-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant"



 
if TARGET_PROCESSOR_TYPE == "f1":
    MCU_DIR    = STM32F1_MCU_DIR
    CMSIS_DIR  = STM32F1_CMSIS_DIR
    HAL_DIR    = STM32F1_HAL_DIR
    DEF_FLAGS  = STM32F1_DEF_FLAGS
    ARCH_FLAGS = STM32F1_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f3":
    MCU_DIR    = STM32F3_MCU_DIR
    CMSIS_DIR  = STM32F3_CMSIS_DIR
    HAL_DIR    = STM32F3_HAL_DIR
    DEF_FLAGS  = STM32F3_DEF_FLAGS
    ARCH_FLAGS = STM32F3_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f4":
    MCU_DIR    = STM32F4_MCU_DIR
    CMSIS_DIR  = STM32F4_CMSIS_DIR
    HAL_DIR    = STM32F4_HAL_DIR
    DEF_FLAGS  = STM32F4_DEF_FLAGS
    ARCH_FLAGS = STM32F4_ARCH_FLAGS

elif TARGET_PROCESSOR_TYPE == "f7":
    MCU_DIR    = STM32F7_MCU_DIR
    CMSIS_DIR  = STM32F7_CMSIS_DIR
    HAL_DIR    = STM32F7_HAL_DIR
    DEF_FLAGS  = STM32F7_DEF_FLAGS
    ARCH_FLAGS = STM32F7_ARCH_FLAGS

else:
    print("ERROR: NOT VALID PROCESSOR TYPE, CHECK MAKE FILE CODE")
    sys.exit(1)



directories = [
    "src/rffw/target/" + TARGET,
    HAL_DIR + "/Src",
    "lib/STM32_USB_Device_Library/Core/Src",
    "lib/STM32_USB_Device_Library/Class/HID/Src",
    "src/rffw/src",
    "src/rffw/src/usb",
    MCU_DIR,
]

excluded_files = [
    ".*_template.c",
]

linkerObjs = ""

INCLUDE_DIRS = [
    "lib/CMSIS/Include",
    CMSIS_DIR,
    "src/rffw/inc",
    "src/rffw/inc/usb",
    HAL_DIR + "/Inc",
    "lib/STM32_USB_Device_Library/Core/Inc",
    "lib/STM32_USB_Device_Library/Class/HID/Inc",
    "src/rffw/target/" + TARGET,
    MCU_DIR,
]
INCLUDES = " ".join("-I" + include for include in INCLUDE_DIRS)

LTO_FLAGS = "-flto -fuse-linker-plugin -O0"
DEBUG_FLAGS = "-ggdb3 -DDEBUG"



CFLAGS = " ".join([
    ARCH_FLAGS,
    LTO_FLAGS,
    DEF_FLAGS,
    DEBUG_FLAGS,
    INCLUDES,
    "-Wunused-parameter -Wdouble-promotion -save-temps=obj -std=gnu99 -Wall -Wextra -Wunsafe-loop-optimizations -ffunction-sections -fdata-sections -MMD -MP"
])

ASMFLAGS = " ".join([
    ARCH_FLAGS,
    "-x assembler-with-cpp",
    INCLUDES,
    "-MMD -MP"
])

mapFile = os.path.join("output", "{OUTPUT_NAME}.map")
linkerDir = os.path.join("src", "rffw", "target")
ldScript = os.path.join("src", "rffw", "target", TARGET_SCRIPT)
LDFLAGS = " ".join([
    "-lm -nostartfiles --specs=nano.specs -lc -lnosys",
    ARCH_FLAGS,
    LTO_FLAGS,
    DEBUG_FLAGS,
    "-static",
    "-Wl,-gc-sections,-Map," + mapFile,
    "-Wl,-L" + linkerDir,
    "-Wl,--cref",
    "-T" + ldScript
])

asm_command = "arm-none-eabi-gcc -c -o output" + os.sep + "{OUTPUT_FILE} " + ASMFLAGS + " {INPUT_FILE}"

compile_command = "arm-none-eabi-gcc -c -o output" + os.sep + "{OUTPUT_FILE} " + CFLAGS + " {INPUT_FILE}"

link_command = "arm-none-eabi-gcc -o output" + os.sep + "{OUTPUT_NAME}.elf {OBJS} " + LDFLAGS

size_command = "arm-none-eabi-size output" + os.sep + "{OUTPUT_NAME}.elf"

copy_obj_command = "arm-none-eabi-objcopy -O binary output" + os.sep + "{OUTPUT_NAME}.elf output" + os.sep + "{OUTPUT_NAME}.bin"


commands = [
]


THREAD_LIMIT = 200
threadLimiter = threading.BoundedSemaphore(THREAD_LIMIT)
locker = threading.Lock()
threadRunning = list()
isStop = False
        
def find_between( s, first, last ):
	try:
		start = s.index( first ) + len( first )
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
        self._stop = threading.Event()


    def run(self):
        threadLimiter.acquire()
        locker.acquire()
        threadRunning.append(self)
        locker.release()
        
        try:
            self.run_command()
        finally:
            locker.acquire()
            threadRunning.remove(self)
            locker.release()
            threadLimiter.release()

    def run_command(self):
        if not self.command:
            return
        locker.acquire()
        if isStop:
            locker.release()
            return
        locker.release()

        locker.acquire()
        print find_between( self.command, "output" + os.sep, ".o" )
        locker.release()

        self.proc = subprocess.Popen(self.command, shell=True)
        stdout_value, stderr_value = self.proc.communicate()
        if self.queue:
            self.queue.put(self.proc.returncode)
        else:
            print proc.returncode
        self.proc = None


    def stop(self):
        if self.proc:
            try:
                self.proc.kill()
            except OSError:
                pass
        self._stop.set()


    def stopped(self):
        return self._stop.isSet()


def FileModified(fileName):
    return True

def AddToList(fileName):
    global commands

    commands.append(fileName) 

def makeObject(fileName):
    head, tail = ntpath.split(fileName)
    root, ext = os.path.splitext(tail)
    if ext.lower() in (".c", ".s"):
        return root + ".o"

    print "Unknown file type: " + tail
    return tail

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

    for directory in directories:
        ProcessList(glob.glob(os.path.join(directory, "*.c")))

    for directory in directories:
        ProcessList(glob.glob(os.path.join(directory, "*.s")))

    queue = Queue.Queue()
    threads = list()
    for command in commands:
        thread = CommandRunnerThread(command=command, queue=queue)
        threads.append(thread)
        
    map(lambda thread: thread.start(), threads)

    while len(threadRunning) > 0:
        try:
            returncode = queue.get(timeout=5)
        except Queue.Empty:
            continue
        if returncode > 0:
            locker.acquire()
            isStop = True
            map(lambda thread: thread.stop(), threadRunning)
            locker.release()
            break

    map(lambda thread: thread.join(), threads)

    if isStop is False: 
        print "linking..."
        link_command = link_command.format(
            OUTPUT_NAME=TARGET,
            OBJS=linkerObjs
        )

        #print link_command
        proc = subprocess.Popen(link_command, shell=True)
        stdout_value, stderr_value = proc.communicate()


        if proc.returncode == 0:
            print "Sizing!"
            size_command = size_command.format(
                OUTPUT_NAME=TARGET
            )

            print size_command
            proc = subprocess.Popen(size_command, shell=True)
            stdout_value, stderr_value = proc.communicate()

        if proc.returncode == 0:
            print "Build succeded copying"
            copy_obj_command = copy_obj_command.format(
                OUTPUT_NAME=TARGET
            )

            print copy_obj_command
            proc = subprocess.Popen(copy_obj_command, shell=True)
            stdout_value, stderr_value = proc.communicate()
     





if __name__ == "__main__":
    main()







