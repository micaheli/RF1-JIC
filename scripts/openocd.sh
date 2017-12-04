#!/bin/bash
openocd -s /usr/local/share/openocd/scripts -f /usr/local/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/local/share/openocd/scripts/target/stm32f4x.cfg

