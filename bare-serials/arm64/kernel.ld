OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
TARGET(binary)

STACKTOP = 0x51000000;

SECTIONS
{
	/*
	 * 入口地址制定为 . = 0x10000; 用下面的启动方式也能运行?
	 * qemu-system-aarch64 -M virt -cpu cortex-a53 \
	 * 	-smp 2 -m 1024 -nographic \
	 * -device loader,addr=0x10000,file=kernel.elf,cpu-num=0
	 *
	 * 但是0x10000对于hw/arm/virt.c板来说不是一个有效的内存空间地址
     * [VIRT_PCIE_ECAM] =          { 0x3f000000, 0x01000000 },
     *  Actual RAM size depends on initial RAM and device memory settings
     * [VIRT_MEM] =                { GiB, LEGACY_RAMLIMIT_BYTES },
	 * 内存空间的起始地址virt_mem = 0x3f000000 + 0x01000000 = 0x40000000
	 *
	 * 所以下面的入口地址设置为0x40010000才是有效的
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
