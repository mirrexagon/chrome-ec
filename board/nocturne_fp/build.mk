# Copyright 2017 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Board specific files build

# the IC is STmicro STM32H743
CHIP:=stm32
CHIP_FAMILY:=stm32h7
CHIP_VARIANT:=stm32h7x3

board-rw=ro_workarounds.o
board-y=board.o fpsensor_detect.o

test-list-y=\
       aes \
       compile_time_macros \
       crc32 \
       flash_physical \
       flash_write_protect \
       mpu \
       mutex \
       pingpong \
       rollback \
       rollback_entropy \
       rtc \
       scratchpad \
       sha256 \
       sha256_unrolled \
       stm32f_rtc \
       utils \
