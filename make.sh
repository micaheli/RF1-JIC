#!/usr/bin/env bash

library_files=(file1 file2 file3)
GCC="arm-none-eabi-gcc -c -o output/"
F4_COMPILE="-mthumb -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion -flto -fuse-linker-plugin -O2 -ggdb3 -DDEBUG -std=gnu99 -Wall -Wextra -Wunsafe-loop-optimizations -Wdouble-promotion -ffunction-sections -fdata-sections -DSTM32F4 -DSTM32F40_41xxx -DHSE_VALUE=8000000 -DFLASH_SIZE=256 -DUSE_STDPERIPH_DRIVER -DREVO -D'__FORKNAME__=\"raceflight\"' -D'__TARGET__=\"REVO\"' -D'__REVISION__=\"00ab78f\"' -save-temps=obj -MMD -MP"



mkdir output


for file in ${library_files[@]}; do
	echo ${GCC}${file}.o $F4_COMPILE librarydir/${file}.c
done

