CC = aarch64-none-linux-gnu-gcc
CFLAGS += -static -pg

run: Image rootfs
	qemu-system-aarch64 -M virt \
		-cpu cortex-a53 -smp 1 -m 1024 \
		-nographic \
		-kernel Image \
		-initrd rootfs \
		-append "root=/dev/ram rdinit=/test"

rootfs.gz : rootfs
	gzip $< -c >$@

rootfs: test
	echo test | cpio -o --format=newc > $@
