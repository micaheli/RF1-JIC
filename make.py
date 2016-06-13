
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import threading
import Queue
import subprocess
import glob
import sys
import ntpath
import ntpath
import os

directories = [
	"src\\rffw\\src",
	"src\\rffw\\inc"
]

excluded_files = [
	"fish_tacos.c"
]

linkerObjs = ""

compile_command = "arm-none-eabi-gcc -o output\\{OUTPUT_FILE} {INPUT_FILE} -mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion -flto -fuse-linker-plugin -O2 -ggdb3 -DDEBUG -std=gnu99 -Wall -Wextra -Wunsafe-loop-optimizations -Wdouble-promotion -ffunction-sections -fdata-sections -DHSE_VALUE=8000000  -MMD -MP"
link_command = "arm-none-eabi-gcc -o bin\\{OUTPUT_NAME}.elf {OBJS} -lm -nostartfiles --specs=nano.specs -lc -lnosys -mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion -flto -fuse-linker-plugin -O2 -ggdb3 -DDEBUG -static -Wl,-gc-sections,-Map,./obj/main/raceflight_KKNGF4_6500.map -Wl,-L./src/rffw/target -Wl,--cref -T./src/rffw/target/STM32F103XB_FLASH.ld"


commands = [
]


locker = threading.Lock()
threadRunning = list()
		


class CommandRunnerThread(threading.Thread):

	def __init__(self, *args, **kwargs):
		self.command = kwargs.pop("command", "")
		self.queue = kwargs.pop("queue", None)
		self.proc = None
		super(CommandRunnerThread, self).__init__(*args, **kwargs) 
		self._stop = threading.Event()

	def run(self):
		locker.acquire()
		threadRunning.append(self)
		locker.release()
		try:
			self.run_command()
		finally:
			locker.acquire()
			threadRunning.remove(self)
			locker.release()


	def run_command(self):
		if not self.command:
			return
		
		self.proc = subprocess.Popen(self.command, shell=True)
		stdout_value, stderr_value = self.proc.communicate()
		if self.queue:
			self.queue.put(self.proc.returncode)
		else:
			print proc.returncode
		self.proc = None

	def stop(self):
		if self.proc:
			self.proc.kill()
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

	return(tail.replace(".c", ".o"))

def ProcessList(fileNames):
	global linkerObjs

	for fileName in fileNames:
		print "filename: " + fileName
		linkerObjs = linkerObjs + " output\\" + makeObject(fileName)
		if FileModified(fileName):
			AddToList(compile_command.replace("{INPUT_FILE}", fileName).replace("{OUTPUT_FILE}", makeObject(fileName)))


def main():
	global link_command

	print "Hi kalyn"
	try:
		os.mkdir("output")
	except:
		pass

	print "Looking for dirs"
	for directory in directories:
		ProcessList(glob.glob(directory + "\\*.c"))


	for command in commands:
		print command



	link_command = link_command.replace("{OUTPUT_NAME}","REVO")
	link_command = link_command.replace("{OBJS}", linkerObjs)


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
			map(lambda thread: thread.stop(), threads)
			break

	map(lambda thread: thread.join(), threads)



if __name__ == "__main__":
	main()







