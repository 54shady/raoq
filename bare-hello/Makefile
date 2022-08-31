# manual compile
#arm-none-eabi-as -mcpu=arm926ej-s -g startup.s -o startup.o
#arm-none-eabi-gcc -c -mcpu=arm926ej-s -g test.c -o test.o
#arm-none-eabi-ld -T test.ld test.o startup.o -o test
#arm-none-eabi-objcopy -O binary test test.bin

CROSS_COMPILE = arm-none-eabi-
include ../common/common.mk

ASFLAGS = -mcpu=arm926ej-s -g
CFLAGS = -mcpu=arm926ej-s -g

# -nostdlib: Do not use the standard system startup files or libraries when linking
LDFLAGS = -T $*.ld -nostdlib

APP = test
DEBUG_APP = test.bin

APP_TARGETS = $(APP)

.PHONY: all
all: $(APP_TARGETS)

test: startup.o

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(APP_TARGETS)
	rm -rf $(DEBUG_APP)

debug: $(DEBUG_APP)
	xterm -e 'qemu-system-arm -M versatilepb -m 128M -s -nographic -S -kernel $(DEBUG_APP).bin'&
	ddd --debugger $(GDB) --command $(DEBUG_APP)-qemu.gdb

# 只有raw格式的程序才需要制定addr参数,其它格式该参数被忽视
# qemu-system-arm -M versatilepb -m 128M -nographic -device loader,file=test
# 使用raw格式时(test.bin)需要指定程序的链接地址
# readelf -l test
# qemu-system-arm -M versatilepb -m 128M -nographic -device loader,file=test.bin,addr=0x10000
run: $(DEBUG_APP)
	qemu-system-arm -M versatilepb -m 128M -nographic -device loader,file=$<,addr=0x10000
