# 裸跑arm程序的方法

## Example01

参考文章:[Run arm program via qemu](https://stackoverflow.com/questions/65896336/how-to-compile-baremetal-hello-world-c-and-run-it-on-qemu-system-aarch64)

参考文章:[Using newlib in baremetal programs](https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-programs/)

有如下三种方法能够实现(下面的实验在x86机器上运行)

1. use a more baremetal-like approach, such as the one described hereafter: it does use sprintf() and the pl011 UART of the qemu-virt machine for displaying the resulting string.

2. use the semihosting mode of qemu, along with the gcc --specs=rdimon.specs with newlib, or using another semihosting library, such as the one available in the Arm Trusted Firmware source code - the example hereafter uses this approach.

3. provide your own syscalls.c , and use the --specs=nosys.specs ld option, so that you can use newlib in baremetal programs: I would suggest to read the excellent article from Francesco Balducci on the Balau blog - the example hereafter uses this approach.

使用的gcc编译器:[gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf](https://developer.arm.com/downloads/-/gnu-a)

Compile program

	aarch64-none-elf-gcc \
		-I. -O0 -ggdb -mtune=cortex-a53 -nostartfiles \
		-ffreestanding --specs=nosys.specs \
		-L. -Wl,-T,qemu-virt-aarch64.ld \
		startup.s pl011.c qemu-virt-aarch64.c syscalls.c -o virt.elf

Run Qemu to test

	qemu-system-aarch64 \
		-machine virt,gic-version=2,secure=on,virtualization=on \
		-cpu cortex-a53 \
		-semihosting -m 128M \
		-nographic -monitor none -serial stdio \
		-kernel virt.elf
