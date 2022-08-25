# u-boot

[参考文章: Using QEMU for Embedded Systems Development, Part 3](https://www.opensourceforu.com/2011/08/qemu-for-embedded-systems-development-part-3/)

[参考文章: U-boot for ARM on QEMU](https://balau82.wordpress.com/2010/03/10/u-boot-for-arm-on-qemu/)

## 编译u-boot并测试

这里需要使用u-boot 1.2.0代码

	git checkout U-Boot-1_2_0 -b U-Boot-1_2_0
	make versatilepb_config arch=ARM CROSS_COMPILE=arm-none-eabi-
	make all arch=ARM CROSS_COMPILE=arm-none-eabi- -j$(nproc)

如果有编译出错,修改(board/versatile/flash.c)改为静态函数

	static void inline spin_wheel (void);

将u-boot加载到0x10000地址启动

	qemu-system-arm -M versatilepb -nographic -kernel u-boot.bin

将u-boot加载到0x0地址启动

	qemu-system-arm -M versatilepb -nographic -device loader,file=u-boot.bin,addr=0x0

## 使用u-boot来启动test测试程序

因为u-boot.bin的大小大约为72kb

	stat -c%s u-boot.bin

所以需要修改链接脚本test.ld对应的连接地址,让程序连接和u-boot在内存中不冲突

```c
ENTRY(_Reset)
SECTIONS
{
 . = 0x100000;
 ... //其他保持不变
 ...
}
```

将test程序打上uboot的头部信息

	mkimage -n "MyTest" -A arm -C none -O linux -T kernel -d test.bin -a 0x00100000 -e 0x00100000 test.uimg
	cat u-boot.bin test.uimg > flash.bin

计算出test程序的起始地址(因为使用qemu的-kernel测试,所以起始地址是0x10000,相应的需要加上这个偏移量)

	printf "0x%X" $(expr $(stat -c%s u-boot.bin) + 65536)
	0x215CC

如果将程序用-device loader加载0x0地址启动则只需要计算u-boot.bin的大小

	printf "0x%X" $(expr $(stat -c%s u-boot.bin))
	0x115CC

加载到0x10000启动测试

	qemu-system-arm -M versatilepb -nographic -kernel flash.bin
	Versatile # bootm 0x215cc

加载到0x0地址启动测试

	qemu-system-arm -M versatilepb -nographic -device loader,file=flash.bin,addr=0x0

进入u-boot后可以查看到程序信息,并启动

	Versatile # iminfo 0x115cc
	Versatile # bootm 0x115cc
