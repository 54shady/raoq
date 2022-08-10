# Run ARM on QEMU (raoq)

## 使用到的gcc编译器

- arm-none-eabi [gcc-arm-10.3-2021.07-x86_64-arm-none-eabi](https://developer.arm.com/downloads/-/gnu-a)

## 非中断模式的串口(bare metal hello world)

[参考文章 Simplest bare metal program for ARM](https://balau82.wordpress.com/2010/02/14/simplest-bare-metal-program-for-arm/)

[参考文章 Hello world for bare metal ARM using QEMU](https://balau82.wordpress.com/2010/02/28/hello-world-for-bare-metal-arm-using-qemu/)

- 下面的实验使用的是QEMU模拟的versatilePB的主板(hw/arm/versatilepb.c)

- 其中UART0[PrimeCell UART (PL011)](https://developer.arm.com/documentation/ddi0183/f/)映射到地址空间的地址为0x101f1000

下面代码是将字符通过串口输出

```c
 /* volatile关键字告诉编译器UART0DR指向的内存内容会变化或在程序运行时发生改变 */
volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

void print_uart0(const char *s) {
	while(*s != '\0') { /* Loop until end of string */
		*UART0DR = (unsigned int)(*s); /* Transmit char */
		s++; /* Next char */
	}
}

void c_entry() {
	print_uart0("Hello world!\n");
}
```

- QEMU中的-kernel选项(一般是加载内核用)将二进制加载到地址0x00010000
- QEMU模拟器一般从0x00000000开始运行代码(一般放置复位向量表, 如果没有的话,外设将不响应中断)
- ARM9第一条指令一般从0x00000000(一般映射到RAM)或0xFFFF0000(一般是ROM)

所以需要将上述代码编译连接后放置在0x00010000地址处, 不需要中断的程序可以不需要设置异常向量表

所以启动脚本可以写成如下(没有处理异常,只是利用硬件的这个机制来写程序)

```c
.global _Reset
_Reset:
LDR sp, =stack_top
BL c_entry
B .
```

链接脚本如下(将程序起始地址设置在0x10000)

```c
ENTRY(_Reset)
SECTIONS
{
 . = 0x10000;
 .startup . : { startup.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x1000; /* 4kB of stack memory */
 stack_top = .;
}
```

使用如下方法编译

	arm-none-eabi-as -mcpu=arm926ej-s -g startup.s -o startup.o
	arm-none-eabi-gcc -c -mcpu=arm926ej-s -g test.c -o test.o
	arm-none-eabi-ld -T test.ld test.o startup.o -o test
	arm-none-eabi-objcopy -O binary test test.bin

使用QEMU测试(ctrl+a, x退出QEMU)

	qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin
