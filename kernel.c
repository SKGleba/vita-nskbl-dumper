//physdump by xyzz
//mod by skgleba
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/io/fcntl.h>

static void *pa2va(unsigned int pa) {
	unsigned int va = 0;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int i;

	for (i = 0; i < 0x100000; i++) {
		vaddr = i << 12;
		__asm__("mcr p15,0,%1,c7,c8,0\n\t"
				"mrc p15,0,%0,c7,c4,0\n\t" : "=r" (paddr) : "r" (vaddr));
		if ((pa & 0xFFFFF000) == (paddr & 0xFFFFF000)) {
			va = vaddr + (pa & 0xFFF);
			break;
		}
	}
	return (void *)va;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	ksceIoMkdir("ur0:ndp/", 6);
	int fd;
	unsigned int *tbl = NULL;
	void *vaddr;
	unsigned int paddr;
	unsigned int i;
	int write;
	unsigned int ttbr0;
	__asm__ volatile ("mrc p15, 0, %0, c2, c0, 0" : "=r" (ttbr0));
	tbl = (unsigned int *) pa2va(ttbr0 & ~0xFF);
	fd = ksceIoOpen("ur0:ndp/nskbl_arzl.bin", SCE_O_WRONLY | SCE_O_TRUNC | SCE_O_CREAT, 6);
	if (fd < 0)
		goto error;
	for (i = 0x500; i < 0x510; i++) {
		tbl[0x3E0] = (i << 20) | 0x10592;
		vaddr = &tbl[0x3E0];
		__asm__ volatile ("dmb sy");
		//LOG("entry: 0x%08X\n", tbl[0x3E0]);
		__asm__ volatile ("mcr p15,0,%0,c7,c14,1" :: "r" (vaddr) : "memory");
		__asm__ volatile ("dsb sy\n\t"
				"mcr p15,0,r0,c8,c7,0\n\t"
				"dsb sy\n\t"
				"isb sy" ::: "memory");
		vaddr = (void *) 0x3E000000;
		__asm__ volatile ("mcr p15,0,%1,c7,c8,0\n\t"
				"mrc p15,0,%0,c7,c4,0\n\t" : "=r" (paddr) : "r" (vaddr));

		write = 0;
		while ((write += ksceIoWrite(fd, (char*)vaddr+write, 0x100000-write)) < 0x100000) {
			if (write < 0) {
				goto error;
			}
		}
	}
error:
	ksceIoClose(fd);
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	return SCE_KERNEL_STOP_SUCCESS;
}
