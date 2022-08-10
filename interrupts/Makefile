# manual compile
#arm-none-eabi-as -mcpu=arm926ej-s -g vectors.s -o vectors.o
#arm-none-eabi-gcc -c -mcpu=arm926ej-s -g test.c -o test.o
#arm-none-eabi-ld -T test.ld test.o vectors.o -o test
#arm-none-eabi-objcopy -O binary test test.bin

CROSS_COMPILE = arm-none-eabi-
include ../common/common.mk

ASFLAGS = -mcpu=arm926ej-s -g
CFLAGS = -mcpu=arm926ej-s -g

# -nostdlib: Do not use the standard system vectors files or libraries when linking
LDFLAGS = -T $*.ld -nostdlib

APP = test
DEBUG_APP = test.bin

APP_TARGETS = $(APP)

.PHONY: all
all: $(APP_TARGETS)

test: vectors.o

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(APP_TARGETS)
	rm -rf $(DEBUG_APP)

debug: $(DEBUG_APP)
	xterm -e 'qemu-system-arm -M versatilepb -m 128M -s -nographic -S -kernel $(DEBUG_APP).bin'&
	ddd --debugger $(GDB) --command $(DEBUG_APP)-qemu.gdb

run: $(DEBUG_APP)
	qemu-system-arm -M versatilepb -m 128M \
		-display none \
		-serial stdio \
		-kernel $<
