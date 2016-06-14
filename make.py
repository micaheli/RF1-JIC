
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import threading
import Queue
import subprocess
import glob
import sys
import ntpath
import os

directories = [
    os.path.join("src", "rffw", "startup"),
    os.path.join("lib", "STM32F1xx_HAL_Driver", "Src"),
    os.path.join("src", "rffw", "src"),
    os.path.join("src", "rffw", "inc"),
]

directories_asm = [
    os.path.join("src", "rffw", "startup"),
    os.path.join("lib", "STM32F1xx_HAL_Driver", "Src"),
    os.path.join("src", "rffw", "src"),
    os.path.join("src", "rffw", "inc"),
]

excluded_files = [
    "fish_tacos.c"
]

linkerObjs = ""

INCLUDE_DIRS = [
    os.path.join("lib", "CMSIS", "Include"),
    os.path.join("lib", "CMSIS", "Device", "ST", "STM32F1xx", "Include"),
    os.path.join("src", "rffw", "inc"),
    os.path.join("lib", "STM32F1xx_HAL_Driver", "Inc"),
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
ldScript = os.path.join("src", "rffw", "target", "stm32_flash_f103_256k.ld")
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

commands = [
]


THREAD_LIMIT = 1
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

    print fileNames
    for fileName in fileNames:
        # fileName = fileName.lower()
        print "filename: " + fileName
        linkerObjs = linkerObjs + " " + os.path.join("output", makeObject(fileName))
        if FileModified(fileName):
            print fileName[-2:]
            if fileName[-2:] == ".s":
                AddToList(asm_command.format(INPUT_FILE=fileName, OUTPUT_FILE=makeObject(fileName)))
            elif fileName[-2:] == ".c":
                AddToList(compile_command.format(INPUT_FILE=fileName, OUTPUT_FILE=makeObject(fileName)))
            else:
                print "YAAAA HOOO"
                AddToList(asm_command.format(INPUT_FILE=fileName, OUTPUT_FILE=makeObject(fileName)))

    print "done"

def main():
    global link_command, isStop

    print "Hi kalyn"
    try:
        os.mkdir("output")
    except:
        pass

    print "Looking for dirs"
    for directory in directories:
        ProcessList(glob.glob(os.path.join(directory, "*.c")))

    for directory in directories:
        ProcessList(glob.glob(os.path.join(directory, "*.s")))


    for command in commands:
        print command

    link_command = link_command.format(
        OUTPUT_NAME="REVO",
        OBJS=linkerObjs
    )

    print "*******************************************************"
    print link_command
    print "*******************************************************"


    commands.append(link_command);

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



if __name__ == "__main__":
    main()







