OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
TARGET(binary)

STACKTOP = 0x51000000;

SECTIONS
{
	/*
	 * 入口地址制定为 . = 0x10000; 时需要如下启动方式
	 * qemu-system-aarch64 -M virt -cpu cortex-a53 \
	 * 	-smp 2 -m 1024 -nographic \
	 * -device loader,addr=0x10000,file=kernel.elf,cpu-num=0
	 *
	 * qemu-system-aarch64 -M virt -cpu cortex-a53 \
	 * 	-smp 2 -m 1024 -nographic \
	 * 	-kernel kernel.elf
	 */
	. = 0x40010000;
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }

	. = STACKTOP;
	stacktop = .;
}
