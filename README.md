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

### 串口编程实例2[(参考文章 Emulating ARM PL011 serial ports)](https://balau82.wordpress.com/2010/11/30/emulating-arm-pl011-serial-ports/)

使用的主板是versatilepb, 三个串口分别映射到地址空间的

    pl011_create(0x101f1000, pic[12], serial_hd(0));
    pl011_create(0x101f2000, pic[13], serial_hd(1));
    pl011_create(0x101f3000, pic[14], serial_hd(2));

	0x101f1000 UART0
	0x101f2000 UART1
	0x101f3000 UART2

测试代码[test.c](./bare-serials/test.c)是将串口输入的字符转换成大写

```c
static inline char upperchar(char c) {
	if((c >= 'a') && (c <= 'z')) {
		return c - 'a' + 'A';
	} else {
		return c;
	}
}
```

编译方法同上,运行测试

	qemu-system-arm -M versatilepb -m 128M -kernel test.bin \
		-serial stdio \
		-serial telnet:localhost:1235,server \
		-serial telnet:localhost:1236,server

在主机上先telnet连接1235端口后再连接1236端口之后就可以按键测试了

	telnet 127.0.0.1 1235
	telnet 127.0.0.1 1236
## 中断模式的串口[参考文章 ARM926 interrupts in QEMU ](https://balau82.wordpress.com/2012/04/15/arm926-interrupts-in-qemu/)

构建异常向量表如下[详细惨考代码vectors.S](./interrupts/vectors.S)

```c
vectors_start:
LDR PC, reset_handler_addr
LDR PC, undef_handler_addr
LDR PC, swi_handler_addr
LDR PC, prefetch_abort_handler_addr
LDR PC, data_abort_handler_addr
B .
LDR PC, irq_handler_addr
LDR PC, fiq_handler_addr
...
vectors_end
```

要让串口支持中断模式,需要在下面三处开启中断

- 在ARM cpsr寄存器中使能中断
- 在中断控制器中使能UART中断
- 在UART的寄存器中使能对应的中断

具体实现代码[test.c 中断模式串口,回显实例](./interrupts/test.c)

```c
#include <stdint.h>

#define UART0_BASE_ADDR 0x101f1000
#define UART0_DR (*((volatile uint32_t *)(UART0_BASE_ADDR + 0x000)))
#define UART0_IMSC (*((volatile uint32_t *)(UART0_BASE_ADDR + 0x038)))

#define VIC_BASE_ADDR 0x10140000
#define VIC_INTENABLE (*((volatile uint32_t *)(VIC_BASE_ADDR + 0x010)))

void __attribute__((interrupt)) irq_handler() {
	/* echo the received character + 1 */
	UART0_DR = UART0_DR + 1;
}

/* all other handlers are infinite loops */
void __attribute__((interrupt)) undef_handler(void) { for(;;); }
void __attribute__((interrupt)) swi_handler(void) { for(;;); }
void __attribute__((interrupt)) prefetch_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) data_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) fiq_handler(void) { for(;;); }

void copy_vectors(void) {
	extern uint32_t vectors_start;
	extern uint32_t vectors_end;
	uint32_t *vectors_src = &vectors_start;
	uint32_t *vectors_dst = (uint32_t *)0;

	while(vectors_src < &vectors_end)
		*vectors_dst++ = *vectors_src++;
}

void main(void) {
	/* enable UART0 IRQ */
	VIC_INTENABLE = 1<<12;
	/* enable RXIM interrupt */
	UART0_IMSC = 1<<4;
	for(;;);
}
```

使用链接脚本[test.ld](./interrupts/test.ld)将程序连接到0x10000地址

```c
ENTRY(vectors_start)
SECTIONS
{
 . = 0x10000;
 .text : {
 vectors.o
 *(.text .rodata)
 }
 .data : { *(.data) }
 .bss : { *(.bss) }
 . = ALIGN(8);
 . = . + 0x1000; /* 4kB of stack memory */
 stack_top = .;
 . = . + 0x1000; /* 4kB of irq stack memory */
 irq_stack_top = .;
}
```

异常向量表需要放置在0地址处,但是程序被加载的时候可能不是在0地址
所以需要进行一个拷贝copy_vectors操作,将异常向量表拷贝到0地址

编译方法同上,测试运行(-kernel选项会把程序加载到0x10000地址运行,所以需要copy_vectors将异常向量表拷贝到0地址)

	qemu-system-arm -M versatilepb -serial stdio -kernel test.bin

上面命令将虚拟机的串口重定向到主机的标准输入输出,此时即可进行中断模式的回显
