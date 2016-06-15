#!/usr/bin/env python
# -*- coding: utf-8 -*-

import glob
import os
import platform
import shlex
import subprocess
import sys
import threading

from multiprocessing import Queue


TARGET = "CC3D"

directories = [
    "src/rffw/startup",
    "lib/STM32F1xx_HAL_Driver/Src",
    "src/rffw/src",
    "src/rffw/inc",
]

directories_asm = [
    "src/rffw/startup",
    "lib/STM32F1xx_HAL_Driver/Src",
    "src/rffw/src",
    "src/rffw/inc",
]

excluded_files = [
    "fish_tacos.c"
]

INCLUDE_DIRS = [
    "lib/CMSIS/Include",
    "lib/CMSIS/Device/ST/STM32F1xx/Include",
    "src/rffw/inc",
    "lib/STM32F1xx_HAL_Driver/Inc",
]
INCLUDES = " ".join("-I" + include for include in INCLUDE_DIRS)

LTO_FLAGS = "-flto -fuse-linker-plugin -O2"
DEBUG_FLAGS = "-ggdb3 -DDEBUG"

ARCH_FLAGS     = "-mthumb -mcpu=cortex-m3"
#F3: ARCH_FLAGS     = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion
#F4: ARCH_FLAGS     = -mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion

CFLAGS = " ".join([
    ARCH_FLAGS,
    LTO_FLAGS,
    DEBUG_FLAGS,
    INCLUDES,
    "-DSTM32F103xB -std=gnu99 -Wall -Wextra -Wunsafe-loop-optimizations -Wdouble-promotion -ffunction-sections -fdata-sections -DHSE_VALUE=8000000  -MMD -MP"
])

ASMFLAGS = " ".join([
    ARCH_FLAGS,
    "-x assembler-with-cpp",
    INCLUDES,
    "-MMD -MP"
])

mapFile = os.path.join("output", "{OUTPUT_NAME}.map")
linkerDir = os.path.join("src", "rffw", "target")
ldScript = os.path.join("src", "rffw", "target", "stm32_flash_f103_128k.ld")
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

copy_obj_command = "arm-none-eabi-objcopy -O binary output" + os.sep + "{OUTPUT_NAME}.elf output" + os.sep + "{OUTPUT_NAME}.bin"


# global objects
commands = []
linkerObjs = ""

THREAD_LIMIT = 16
threadLimiter = threading.BoundedSemaphore(THREAD_LIMIT)
locker = threading.Lock()
threadRunning = list()
isStop = False


class CommandRunnerThread(threading.Thread):

    def __init__(self, *args, **kwargs):
        self.command = kwargs.pop("command", "")
        self.queue = kwargs.pop("queue", None)
        self.proc = None
        super(CommandRunnerThread, self).__init__(*args, **kwargs)
        self.stop = threading.Event()

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

        print(self.command)
        self.proc = subprocess.Popen(shlex.split(self.command))
        stdout_value, stderr_value = self.proc.communicate()
        if self.queue:
            self.queue.put(self.proc.returncode)
        else:
            print(proc.returncode)
        self.proc = None

    def stop(self):
        if self.proc:
            try:
                self.proc.kill()
            except OSError:
                pass
        self.stop.set()

    def stopped(self):
        return self._stop.isSet()


def FileModified(fileName):
    return True

def AddToList(fileName):
    global commands

    commands.append(fileName)

def makeObject(fileName):
    head, tail = os.path.split(fileName)
    root, ext = os.path.splitext(tail)
    if ext.lower() in (".c", ".s"):
        return root + ".o"

    print("Unknown file type: " + tail)
    return tail

def ProcessList(fileNames):
    global linkerObjs

    for fileName in fileNames:
        linkerObjs = linkerObjs + " " + os.path.join("output", makeObject(fileName))
        if FileModified(fileName):
            if fileName[-2:] == ".s":
                AddToList(asm_command.format(INPUT_FILE=fileName, OUTPUT_FILE=makeObject(fileName)))
            elif fileName[-2:] == ".c":
                AddToList(compile_command.format(INPUT_FILE=fileName, OUTPUT_FILE=makeObject(fileName)))
            else:
                AddToList(asm_command.format(INPUT_FILE=fileName, OUTPUT_FILE=makeObject(fileName)))


def main():
    global link_command, isStop, copy_obj_command

    if platform.system() == 'Windows':
        for index, object in enumerate(directories):
            directories[index] = object.replace("/", "\\")

        for index, object in enumerate(directories_asm):
            directories_asm[index] = object.replace("/", "\\")

        for index, object in enumerate(INCLUDE_DIRS):
            INCLUDE_DIRS[index] = object.replace("/", "\\")

    if not os.path.exists("output"):
        os.mkdir("output")

    for directory in directories:
        ProcessList(glob.glob(os.path.join(directory, "*.c")))

    for directory in directories:
        ProcessList(glob.glob(os.path.join(directory, "*.s")))

    queue = Queue()
    threads = list()
    for command in commands:
        thread = CommandRunnerThread(command=command, queue=queue)
        threads.append(thread)

    list(map(lambda thread: thread.start(), threads))

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

    list(map(lambda thread: thread.join(), threads))

    if isStop is False:
        print("linking...")
        link_command = link_command.format(
            OUTPUT_NAME=TARGET,
            OBJS=linkerObjs
        )

        print(link_command)
        proc = subprocess.Popen(shlex.split(link_command))
        stdout_value, stderr_value = proc.communicate()

        if proc.returncode == 0:
            print("Build succeded copying")
            copy_obj_command = copy_obj_command.format(
                OUTPUT_NAME=TARGET
            )

            print(copy_obj_command)
            proc = subprocess.Popen(shlex.split(copy_obj_command))
            stdout_value, stderr_value = proc.communicate()


if __name__ == "__main__":
    main()
