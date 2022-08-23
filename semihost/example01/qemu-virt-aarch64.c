#include <stdio.h>

#include "pl011.h"
#include "syscalls.h"

/* angel/semihosting interface */
#define SYS_WRITE0                       0x04
static uint64_t semihosting_call(uint32_t operation, uint64_t parameter)
{
	__asm("HLT #0xF000");
}

void main()
{
	char buffer[BUFSIZ];
	uint64_t regCurrentEL;

	__asm volatile ("mrs %0, CurrentEL" : "=r" (regCurrentEL));

	/* 1. 直接将输出的数据发送到串口中, 最直接的裸机程序 */
	sprintf(buffer, "Hello EL%d World!\n", (regCurrentEL >> 2) & 0b11);
	puts_uart0(buffer);

	/*
	 * 2. 使用qemu中模拟的arm semihosting接口
	 * angel/semihosting interface
	 */
	sprintf(buffer, "Hello semi-hosted EL%d World!\n", (regCurrentEL >> 2) & 0b11);
	semihosting_call(SYS_WRITE0, (uint64_t) (uintptr_t)  buffer);

	/*
	 * 3. 提供自己的实现的syscalls.c
	 * 并在编译程序时使用gcc的--specs=nosys.specs链接选项来使用newlib
	 * newlib -  custom syscalls.c, with _write() using UART0
	 */
	printf("Hello EL%d World! (syscalls version)\n", (regCurrentEL >> 2) & 0b11);
}
