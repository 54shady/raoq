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
